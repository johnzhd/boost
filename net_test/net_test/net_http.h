#pragma once

#include "net_tcp.h"
#include "http_work.h"

#include <assert.h>
#include <algorithm>


class net_http
{
public:
	net_http(void)
	{
	};
	~net_http(void){
		clear();
	};
	void clear()
	{
		url_info.clear();
		net_ptr = nullptr;
		net_ssl_ptr = nullptr;
		http_parser_ptr = nullptr;
	}
public:

	// url 
	net_url url_info;

	// net work ...
	boost::shared_ptr<net_tcp<net_base<>>> net_ptr;
	boost::shared_ptr<net_tcp<net_ssl_base<>>> net_ssl_ptr;

	// http parser
	boost::shared_ptr<http_work<>> http_parser_ptr;

	// get result

public:

	void test(boost::asio::yield_context yield, boost::shared_ptr<boost::asio::io_service> io_ptr, std::string url);

	// http parser

	// get result
};

