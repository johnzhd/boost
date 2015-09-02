#pragma once

#include "net_api.hpp"


class net_work
{
public:
	net_work(boost::shared_ptr<net_thread> t);
	~net_work();
protected:
	boost::shared_ptr<net_thread> thread_opt;
	boost::asio::io_service& get_io();
public:
	void stop_signal(int s);
public:
	void demo_loop(boost::asio::yield_context yc);
};




class task_work
{
public:
	task_work();
	~task_work();
public:
};

