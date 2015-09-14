#pragma once

#include <vector>
#include <list>
#include <iostream>
#include <fstream>

#include "task_work.h"


template<typename T_work_base = task_work_base>
class get_url_work
{
public:
	T_work_base base_core;
public:
	get_url_work(boost::shared_ptr<net_thread> net)
		:base_core(net), b_endpoint(false), host_endpoint()
	{}
	~get_url_work() {}
public:
	bool start_work()
	{
		task_urls.push_back("http://domain.com/downloadxml.php");
		boost::asio::spawn(base_core.get_io(), boost::bind(&get_url_work<T_work_base>::work_loop_enter, this, _1));


		return true;
	}
	void work_loop_enter(boost::asio::yield_context yc);
	void timer_loop_enter(boost::asio::yield_context yc) {};
protected:
	boost::asio::ip::tcp::endpoint host_endpoint;
	bool b_endpoint;
public:
	std::string get_task_url()
	{
		if (task_urls.empty())
			return std::string();
		auto ret = task_urls.front();
		task_urls.pop_front();
		return ret;
	}
	std::list<std::string> task_urls;
protected:
	void work_html(std::vector<char>& v);
};

template<typename T_work_base>
inline void get_url_work<T_work_base>::work_loop_enter(boost::asio::yield_context yc)
{
	boost::system::error_code ec;
	std::string url, schema, domain, port, path, param;
	std::vector<char> response;
	size_t n;
	while (!ec)
	{
		url = get_task_url();
		if (url.empty())
			break;// end work

		schema.clear();
		domain.clear();
		port.clear();
		path.clear();
		param.clear();
		if (false == url_parser(url, schema, domain, port, path, param))
			continue;

		url = http_request_url(schema, domain, port, path, param, false);
		if (url.empty())
			continue;

		if (!b_endpoint)
		{
			host_endpoint = base_core.get_endpoint(yc, ec, domain, port);
			if (ec)
				break;
			b_endpoint = true;
		}

		auto s = base_core.make_socket(schema == "https" ? HTTPS : HTTP);
		if (!s)
			break;

		if (!s->connect(yc, ec, host_endpoint) || ec)
		{
			ec.message();
			continue;
		}

		if (0 == s->send_at_least(yc, ec, url.length(), url) || ec)
		{
			ec.message();
			continue;
		}

		http_parser_api hp;
		response.resize(2048, 0);
		while (!ec && !hp.is_err() && !hp.is_done())
		{
			if (0 == (n = s->recv(yc, ec, response)) || ec)
			{
				break;
			}
			hp.push(&response[0], n);
		}

		response.clear();
		if (hp.is_done() && hp.search_head("Content-Type", "text/"))
		{
			if (hp.search_head("Content-Encoding", "gzip"))
			{
				gzip_unzip un;
				for (auto& v : hp.body_list)
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
				for (auto& v : hp.body_list)
				{
					std::copy(v.begin(), v.end(), std::back_inserter(response));
				}
			}

			if (!response.empty())
			{
				work_html(response);
			}
		}

		if (hp.is_err())
		{
			hp.get_err();
		}

		if (ec)
		{
			ec.message();
		}
	}

}

template<typename T_work_base>
inline void get_url_work<T_work_base>::work_html(std::vector<char>& v)
{
	{
		std::ofstream f("1.xml");
		for (auto c : v)
		{
			f << c;
		}
	}
	v.push_back(0);
	xml_parser x(&v[0]);
	if (!x.get_last_err().empty())
	{
		x.get_last_err();
		return;
	}
	auto list = x.core.get_child("output.entries");
	std::string target, url, response;
	for (auto& line : list)
	{
		response = line.second.get("response", "");
		if (response != "alive")
		{
			continue;
		}
		target = line.second.get("target", "");
		url = line.second.get("url", "");

		std::cout << url << "\t\t-|-\t\t" << target << "\n";
	}
}
