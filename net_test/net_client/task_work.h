#pragma once

#include <vector>
#include <string>

#include "net_api.hpp"

#include "parser_tools.h"
#include "http_parser_api.h"
#include "http_maker.hpp"
#include "gzip_api.h"


class task_work_base
{
public:
	task_work_base(boost::shared_ptr<net_thread> net);
	virtual ~task_work_base();
protected:
	boost::shared_ptr<net_thread> thread_opt;
	boost::shared_ptr<boost::asio::ip::tcp::resolver> dns_opt;
	boost::system::error_code g_ec;
public:
	std::string get_last_err();
public:
	boost::shared_ptr<boost::asio::ip::tcp::resolver> get_dns();

	template<typename T>
	boost::asio::ip::tcp::endpoint make_endpoint(const T& ip, const T& port)
	{
		return to_endpoint(ip, port);
	}

	boost::asio::ip::tcp::endpoint get_endpoint(boost::asio::yield_context yc, boost::system::error_code&ec, std::string ip, std::string port);

	boost::shared_ptr<b_net_socket_api> make_socket(protocal_type type);
public:
	boost::asio::io_service& get_io();
};
