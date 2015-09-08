#pragma once

#include <vector>
#include <string>

#include "net_api.hpp"

// this class is a demo
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



// this class is demo2
class task_work
{
public:
	task_work(boost::shared_ptr<net_thread> net);
	~task_work();
protected:
	boost::shared_ptr<net_thread> thread_opt;
	boost::shared_ptr<boost::asio::ip::tcp::resolver> dns_opt;
	boost::system::error_code g_ec;
public:
	void stop_signal(int s);
public:
	void work_loop(boost::asio::yield_context yc);
	void timer_loop(boost::asio::yield_context yc);
public:
	std::string get_last_err();
public:
	std::vector<std::string> task_list;
	std::string get_task();
	size_t get_task_remain();
};








