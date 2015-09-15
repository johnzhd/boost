#include "stdafx.h"
#include "task_work.h"

#include "gzip_api.h"
#include "net_tools.hpp"
#include "http_maker.hpp"
#include "http_parser_api.h"
#include "parser_tools.h"

#include <assert.h>


task_work_base::task_work_base(boost::shared_ptr<net_thread> net)
	:thread_opt(net)
{
}

task_work_base::~task_work_base()
{
}

std::string task_work_base::get_last_err()
{
	return g_ec.message();
}

boost::shared_ptr<boost::asio::ip::tcp::resolver> task_work_base::get_dns()
{
	if (!dns_opt)
		dns_opt = thread_opt->make_resolver();
	return dns_opt;
}

boost::asio::ip::tcp::endpoint task_work_base::get_endpoint(boost::asio::yield_context yc, boost::system::error_code & ec, std::string ip, std::string port)
{
	boost::asio::ip::tcp::endpoint end = to_endpoint(ip, port);
	if (end.port() != 0 || end.address().to_v4().to_ulong() != 0)
	{
		return end;
	}
	if (!get_dns())
		return end;
	return dns(yc, ec, *get_dns(), ip, port);
}

boost::shared_ptr<b_net_socket_api> task_work_base::make_socket(protocal_type type)
{
	return thread_opt->make_socket(type);
}

boost::asio::io_service& task_work_base::get_io()
{
	return *thread_opt->get_io();
}

boost::shared_ptr<b_net_acceptor_api> task_work_base::make_acceptor()
{
	return thread_opt->make_acceptor();
}

