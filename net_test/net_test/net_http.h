#pragma once

#include "net_base.hpp"
#include "net_ssl_base.hpp"
#include "http_work.h"

#include <assert.h>

class net_url
{
public:
	net_url(void);
	~net_url(void);
protected:
	// url
	std::string url_buff;
	http_parser_url m_url;
public:
	bool parser_url(std::string url, enum http_parser_type type = HTTP_REQUEST)
	{
		url_buff = url;
		return 0 == http_parser_parse_url( url_buff.c_str(), url_buff.length(), (type == HTTP_REQUEST), &m_url);
	}
	/*
	UF_SCHEMA           = 0
	, UF_HOST             = 1
	, UF_PORT             = 2
	, UF_PATH             = 3
	, UF_QUERY            = 4
	, UF_FRAGMENT         = 5
	, UF_USERINFO         = 6
	, UF_MAX              = 7
	*/
	template<enum http_parser_url_fields field>
	std::string get_infomation()
	{
		if ( m_url.field_set | (1<<field) )
		{
			return url_buff.substr(m_url.field_data[field].off,m_url.field_data[field].len);
		}
		return std::string();
	}
};


class net_http
{
public:
	net_http(void)
		:net_io(boost::make_shared<boost::asio::io_service>())
	{
	};
	~net_http(void);

protected:

	// url 
	net_url url_info;

	// net work ...
	boost::shared_ptr<boost::asio::io_service> net_io;
	boost::shared_ptr<net_ssl_base<>> net_ssl_socket;
	boost::shared_ptr<net_base<>> net_socket;
	boost::shared_ptr<boost::asio::ip::tcp::acceptor> net_acceptor;

	// http parser
	http_work<> http_parser;

	// get result

public:

	// net work ...
	boost::shared_ptr<boost::asio::ip::tcp::acceptor> new_acceptor()
	{
		assert(net_io);
		return boost::make_shared<boost::asio::ip::tcp::acceptor>(*net_io);
	}

	boost::shared_ptr<boost::asio::deadline_timer> new_timer(boost::posix_time::ptime t)
	{
		assert(net_io);
		return boost::make_shared<boost::asio::deadline_timer>(*net_io, t);

		// t->async_wait(boost::bind(timer_show,_1,t)); 

		//t->expires_at(t->expires_at() + boost::posix_time::seconds(5)); 
		//t->async_wait(boost::bind(timer_show, _1, t));
	}

	boost::asio::ip::tcp::resolver::iterator yield_dns(boost::asio::yield_context yield,
		boost::system::error_code& ec,
		std::string address,
		std::string port)
	{
		boost::asio::ip::tcp::resolver resolver(*net_io);
		boost::asio::ip::tcp::resolver::query query(address, port);
		return resolver.async_resolve(query,yield[ec]);
	}

	void accepter_bind( boost::system::error_code &ec,
		boost::shared_ptr<boost::asio::ip::tcp::acceptor> ptr_,
		boost::asio::ip::tcp::endpoint local_endpoint_)
	{
		ptr_->open(local_endpoint_.protocol(),ec);
		ptr_->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true),ec);
		ptr_->bind(local_endpoint_,ec);
		ptr_->listen(boost::asio::socket_base::max_connections,ec);
	}

	template<typename SOCKET_PTR, typename BYTE_CONTAINER>
	size_t yield_send(boost::asio::yield_context yield,
		boost::system::error_code &ec,
		SOCKET_PTR s,
		BYTE_CONTAINER& buff)
	{
		return s->async_write_some(boost::asio::buffer(buff), yield[ec] );
	}

	template<typename SOCKET_PTR, typename BYTE_CONTAINER>
	size_t yield_send_hold(boost::asio::yield_context yield,
		boost::system::error_code &ec,
		SOCKET_PTR s,
		BYTE_CONTAINER& buff)
	{
		return boost::asio::async_write(s,
			boost::asio::buffer(buff),
			boost::asio::transfer_at_least(buff.size()), yield[ec] );
	}

	template<typename SOCKET_PTR, typename BYTE_CONTAINER>
	size_t yield_recv(boost::asio::yield_context yield,
		boost::system::error_code &ec,
		SOCKET_PTR s,
		BYTE_CONTAINER& buff)
	{
		return s->async_read_some(boost::asio::buffer(buff), yield[ec] );
	}

	// http parser

	// get result
};

