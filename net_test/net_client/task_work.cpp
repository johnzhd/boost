#include "stdafx.h"
#include "task_work.h"


#include "net_tools.h"



net_work::net_work(boost::shared_ptr<net_thread> t)
	:thread_opt(t)
{
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


task_work::task_work()
{
}


task_work::~task_work()
{
}




