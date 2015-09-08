#include "stdafx.h"
#include "task_work.h"

#include "gzip_api.h"
#include "net_tools.h"
#include "http_maker.hpp"
#include "http_parser_api.h"
#include "parser_tools.h"

#include <assert.h>


net_work::net_work(boost::shared_ptr<net_thread> t)
	:thread_opt(t)
{
	assert(thread_opt);
}

net_work::~net_work()
{
}

boost::asio::io_service & net_work::get_io()
{
	// TODO: 在此处插入 return 语句
	return *thread_opt->get_io();
}

void net_work::stop_signal(int s)
{
	s;
}

void net_work::demo_loop(boost::asio::yield_context yc)
{
	protocal_type ptl(HTTPS);
	boost::system::error_code ec;
	boost::shared_ptr<b_net_socket_api> opt;
	std::string ip, port, packet;
	ip = "180.97.33.108";
	port = "443";
	packet = "GET / HTTP/1.1\r\nAccept: *.*\r\nHost: www.baidu.com:443\r\n\r\n";
	make_shared(opt, ptl, thread_opt->get_io());

	if (!opt->connect(yc, ec, to_endpoint(ip, port)))
	{
		return;
	}

	if (0 == opt->send_at_least(yc, ec, packet.length(), packet))
	{
		return;
	}

	std::vector<char> v;
	v.resize(2048);
	if (0 == opt->recv(yc, ec, v))
	{
		return;
	}

}


task_work::task_work(boost::shared_ptr<net_thread> net)
	:thread_opt(net)
{
}


task_work::~task_work()
{
}

void task_work::stop_signal(int s)
{
	s;
	// log
	// *stop timer work
	// clear 
}

void task_work::work_loop(boost::asio::yield_context yc)
{
	std::string url, schema, domain, port, path, param;
	std::string request;
	std::vector<char> response;
	size_t n;
	boost::system::error_code ec;
	response.resize(2048);
	while (get_task_remain() > 0)
	{
		url = get_task();
		if (url.empty())
			continue;
		if (!url_parser(url, schema, domain, port, path, param))
		{
			continue;
		}

		auto ep = dns(yc, ec, *dns_opt, domain, port);

		auto s = thread_opt->make_socket(schema=="https"? HTTPS : HTTP);
		if (false == s->connect(yc, ec, ep))
		{
			continue;
		}

		request = http_request_url(schema,domain,port,path,param, false);
		if (0 == s->send_at_least(yc, ec, request.length(), request) || ec)
		{
			continue;
		}

		http_parser_api parser_opt;
		while (!ec && !parser_opt.is_done() && !parser_opt.is_err())
		{
			if (0 == (n = s->recv(yc, ec, response)) || ec)
			{
				break;
			}

			parser_opt.push(&response[0], n);
			if (parser_opt.is_err())
			{
				//
				parser_opt.get_err();
				break;
			}
		}

		response.clear();
		if (parser_opt.is_done())
		{
			// Content-Encoding: gzip 
			// Content-Type: application/x-gzip
			if (parser_opt.search_head("Content-Encoding", "gzip"))
			{
				gzip_unzip un;
				for (auto& v : parser_opt.body_list)
				{
					un.push((const byte*)&v[0], v.size());
				}
				un.push_end();

				if (!un.copy_result(response))
				{
					response.clear();
				}
			}
			else
			{
				for (auto& v : parser_opt.body_list)
				{
					std::copy(v.begin(), v.end(), std::back_inserter(response));
				}
			}
		}

		if (!response.empty() && parser_opt.search_head("Content-Type", "text/"))
		{
			// html
			if (response[0] == '<')
			{
				// nothing
			}
			// json
			else if (response[0] == '{')
			{
				json_parser j(&response[0], response.size());
				if (!j.get_last_err().empty())
				{
					continue;
				}
				try
				{
					auto rows = j.core.get_child("rows");
					for (auto it = rows.begin(); it != rows.end(); it++)
					{
						it->first;
						auto ret = it->second.get("cell.fund_id", "");
					}
				}
				catch (const boost::property_tree::json_parser_error& e)
				{
					e.message();
				}
			}
		}
	}
}

void task_work::timer_loop(boost::asio::yield_context yc)
{
	auto timer_opt = thread_opt->make_timer();
	auto t = make_time(0, 0, 5, 0, 0);
	boost::system::error_code ec;
	while (!ec)
	{
		thread_opt->wait_time(timer_opt, t, yc, ec);

		// print tasks count
		// std::cout << get_task_remain() << "\n";
	}
}

std::string task_work::get_last_err()
{
	return g_ec.message();
}

std::string task_work::get_task()
{
	if(task_list.empty())
		return std::string();
	auto a = task_list.back();
	task_list.pop_back();
	return a;
}

size_t task_work::get_task_remain()
{
	return task_list.size();
}




