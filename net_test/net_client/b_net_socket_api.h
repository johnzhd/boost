#pragma once

#include <vector>

#include "net_tools.h"
#include "boost_net_base.hpp"
#include "boost_net_socket.hpp"

class protocal_socket
{
public:
	boost::shared_ptr<http_base> http_opt;
	boost::shared_ptr<https_base> https_opt;
public:
	protocal_socket(boost::shared_ptr<http_base> h);
	protocal_socket(boost::shared_ptr<https_base> hs);
	protocal_socket();
};

class b_net_socket_api : public boost::enable_shared_from_this<b_net_socket_api>
{
public:
	b_net_socket_api(protocal_type type, boost::shared_ptr<boost::asio::io_service> io);
	~b_net_socket_api();

	// Base
public:
	boost::shared_ptr<boost::asio::io_service> io();
protected:
	boost::shared_ptr<boost::asio::io_service> io_opt;

	// Socket
public:
	bool get_socket(boost::shared_ptr<http_base>& ret);
	bool get_socket(boost::shared_ptr<https_base>& ret);
protected:
	// socket work opt
	const protocal_type s_type;
	protocal_socket s_opt;

	// out of time opt
public:
	typedef enum dlt_type
	{
		dlt_connect = 0,
		dlt_send,
		dlt_send_at_least,
		dlt_recv,

		dlt_max
	}dlt_type;
	bool set_dlt_time(dlt_type no, size_t delay);
protected:
	inline size_t get_dlt_time(dlt_type no);
	std::vector<size_t> v_dlt;
	boost::shared_ptr<boost::asio::deadline_timer> dlt_opt;


public:

	bool connect(boost::asio::yield_context yc, boost::system::error_code& ec, boost::asio::ip::tcp::endpoint ep);


	template<typename... ARGS>
	size_t send(boost::asio::yield_context yc, boost::system::error_code& ec, const ARGS&...args)
	{
		size_t ret = 0;
		start_deadline(dlt_send, ec);
		if (ec)
			return ret;
		my_RAII<decltype(cancel_deadline_func)> raii(cancel_deadline_func);
		switch (s_type)
		{
		case HTTP:
			assert(s_opt.http_opt);
			ret = s_opt.http_opt->send(yc, ec, args...);
			break;
		case HTTPS:
			assert(s_opt.https_opt);
			ret = s_opt.https_opt->send(yc, ec, args...);
			break;
		default:
			assert(false);
			break;
		}
		return ret;
	}

	template<typename... ARGS>
	size_t send_at_least(boost::asio::yield_context yc, boost::system::error_code& ec, size_t at_least_size, const ARGS&...args)
	{
		size_t ret = 0;
		start_deadline(dlt_send, ec);
		if (ec)
			return ret;
		my_RAII<decltype(cancel_deadline_func)> raii(cancel_deadline_func);
		switch (s_type)
		{
		case HTTP:
			assert(s_opt.http_opt);
			ret = s_opt.http_opt->send_at_least(yc, ec, at_least_size, args...);
			break;
		case HTTPS:
			assert(s_opt.https_opt);
			ret = s_opt.https_opt->send_at_least(yc, ec, at_least_size, args...);
			break;
		default:
			assert(false);
			break;
		}
		return ret;
	}

	template<typename... ARGS>
	size_t recv(boost::asio::yield_context yc, boost::system::error_code& ec, ARGS&...args)
	{
		size_t ret = 0;
		start_deadline(dlt_recv, ec);
		if (ec)
			return ret;
		my_RAII<decltype(cancel_deadline_func)> raii(cancel_deadline_func);
		switch (s_type)
		{
		case HTTP:
			assert(s_opt.http_opt);
			ret = s_opt.http_opt->recv(yc, ec, args...);
			break;
		case HTTPS:
			assert(s_opt.https_opt);
			ret = s_opt.https_opt->recv(yc, ec, args...);
			break;
		default:
			assert(false);
			break;
		}
		return ret;
	}

	void cancel(void);

	const boost::function<void(void)> cancel_deadline_func;
protected:
	void start_deadline(dlt_type no, boost::system::error_code& ec);
	void deadline(boost::system::error_code ec);
	void cancel_deadline();


public:
	bool get_extern_info(std::vector<unsigned char>& ret);
};

