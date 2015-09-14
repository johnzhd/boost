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
	b_net_socket_api(protocal_type type, boost::shared_ptr<boost::asio::io_service> io, boost::shared_ptr<boost::asio::ssl::context> ctx = boost::shared_ptr<boost::asio::ssl::context>());
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
	protocal_type get_type();
protected:
	// socket work opt
	const protocal_type s_type;
	protocal_socket s_opt;

	// out of time opt
public:
	typedef enum dlt_type
	{
		dlt_connect = 0,
		dlt_handshake,
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

	// here must be a function obj
	// and its life circil must cover the whole connect action
	// otherwise there is a memory err occur when handler call
	// so create a boost functional to content lambda or other type
	//void connect(boost::function<void(boost::system::error_code)>& handler, boost::asio::ip::tcp::endpoint ep);

	template<typename Func>
	void connect(Func handler, boost::asio::ip::tcp::endpoint ep)
	{
		bool ret = false;
		start_deadline(dlt_connect);
		my_RAII<decltype(cancel_deadline_func)> raii(cancel_deadline_func);
		switch (s_type)
		{
		case HTTP:
			assert(s_opt.http_opt);
			s_opt.http_opt->connect(handler, ep);
			ret = !ec;
			break;
		case HTTPS:
			assert(s_opt.https_opt);
			s_opt.https_opt->connect(handler, ep);
			ret = !ec;
			break;
		default:
			assert(false);
			break;
		}
		return ret;
	}

	template<typename Func>
	void handshake(Func handler, boost::asio::ssl::stream_base::handshake_type t = boost::asio::ssl::stream_base::client)
	{
		start_deadline(dlt_handshake);
		my_RAII<decltype(cancel_deadline_func)> raii(cancel_deadline_func);
		switch (s_type)
		{
		case HTTP:
			assert(s_opt.http_opt);
			s_opt.http_opt->handshake(handler, t);
			break;
		case HTTPS:
			assert(s_opt.https_opt);
			s_opt.https_opt->handshake(handler, t);
			break;
		default:
			assert(false);
			break;
		}
	}

	template<typename Func, typename... ARGS>
	size_t send(Func handler, const ARGS&...args)
	{
		size_t ret = 0;
		start_deadline(dlt_send);
		my_RAII<decltype(cancel_deadline_func)> raii(cancel_deadline_func);
		switch (s_type)
		{
		case HTTP:
			assert(s_opt.http_opt);
			ret = s_opt.http_opt->send(handler, args...);
			break;
		case HTTPS:
			assert(s_opt.https_opt);
			ret = s_opt.https_opt->send(handler, args...);
			break;
		default:
			assert(false);
			break;
		}
		return ret;
	}

	template<typename... ARGS>
	size_t send(boost::asio::yield_context yc, boost::system::error_code& ec, const ARGS&...args)
	{
		return send(yc[ec], args...);
		/*
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
		*/
	}
	
	template<typename Func, typename... ARGS>
	size_t send_at_least(Func handler, size_t at_least_size, const ARGS&...args)
	{
		size_t ret = 0;
		start_deadline(dlt_send);
		my_RAII<decltype(cancel_deadline_func)> raii(cancel_deadline_func);
		switch (s_type)
		{
		case HTTP:
			assert(s_opt.http_opt);
			ret = s_opt.http_opt->send_at_least(handler, at_least_size, args...);
			break;
		case HTTPS:
			assert(s_opt.https_opt);
			ret = s_opt.https_opt->send_at_least(handler, at_least_size, args...);
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
		return send_at_least(yc[ec], at_least_size, args...);
		/*
		size_t ret = 0;
		start_deadline(dlt_send);
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
		*/
	}

	template<typename Func, typename... ARGS>
	size_t recv(Func handler, ARGS&...args)
	{
		size_t ret = 0;
		start_deadline(dlt_recv);
		my_RAII<decltype(cancel_deadline_func)> raii(cancel_deadline_func);
		switch (s_type)
		{
		case HTTP:
			assert(s_opt.http_opt);
			ret = s_opt.http_opt->recv(handler, args...);
			break;
		case HTTPS:
			assert(s_opt.https_opt);
			ret = s_opt.https_opt->recv(handler, args...);
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
		return recv(yc[ec], args...);
		/*
		size_t ret = 0;
		start_deadline(dlt_recv);
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
		*/
	}

	void cancel(void);

	const boost::function<void(void)> cancel_deadline_func;
protected:
	bool start_deadline(dlt_type no);
	void deadline(boost::system::error_code ec);
	void cancel_deadline();


public:
	bool get_extern_info(std::vector<unsigned char>& ret);
};


class b_net_acceptor_api : public boost::enable_shared_from_this<b_net_acceptor_api>
{
public:
	b_net_acceptor_api(boost::shared_ptr<boost::asio::io_service> io)
		:io_opt(io), cancel_deadline_func(boost::bind(&b_net_acceptor_api::cancel_deadline, this))
	{
		make_shared(base_core, *io_opt);
		make_shared(dlt_opt, *io_opt);
	}
	~b_net_acceptor_api() {};
public:
	boost::shared_ptr<boost::asio::io_service> io()
	{
		return io_opt;
	}
	void set_dlt_time(size_t t)
	{
		dlt_time = t;
	}
	size_t get_dlt_time()
	{
		return dlt_time;
	}
	boost::shared_ptr<boost::asio::ssl::context> get_ctx()
	{
		if (!ctx_opt)
		{
			boost::system::error_code ec;
			ctx_opt = base_core->new_context(ec);
			if (ec)
			{
				assert(false);
				throw ec;
			}
		}
		return ctx_opt;
	}

	bool start_deadline()
	{
		if (!dlt_opt)
			return false;
		boost::system::error_code ec;
		dlt_opt->expires_from_now(boost::posix_time::seconds(dlt_time), ec);
		if (ec)
			return false;
		dlt_opt->async_wait(boost::bind(&b_net_acceptor_api::deadline, shared_from_this(), _1));
		return true;
	}

	void deadline(boost::system::error_code ec)
	{
		if (!ec)
		{
			cancel();
		}
	}

	void cancel_deadline()
	{
		boost::system::error_code ec;
		if (dlt_opt)
			dlt_opt->cancel(ec);
	}

	void cancel()
	{
		if (base_core)
		{
			base_core->cancel();
		}
	}

	const boost::function<void(void)> cancel_deadline_func;
protected:
	boost::shared_ptr<boost::asio::io_service> io_opt;
	boost::shared_ptr<http_server_base> base_core;
	size_t dlt_time;
	boost::shared_ptr<boost::asio::deadline_timer> dlt_opt;
	boost::shared_ptr<boost::asio::ssl::context> ctx_opt;
public:
	boost::shared_ptr<b_net_socket_api> make_server_socket(protocal_type type)
	{
		boost::shared_ptr<b_net_socket_api> ret;
		make_shared(ret, type, io(), get_ctx());
		return ret;
	}
public:
	bool bind(boost::system::error_code& ec, boost::asio::ip::tcp::endpoint ep)
	{
		base_core->bind_listen(ec, ep);
		return !ec;
	}

	template<typename Func>
	bool accept(Func handler, boost::shared_ptr<b_net_socket_api>& socket)
	{
		if (!socket)
			return false;

		switch (socket->get_type())
		{
		case HTTP:
		{
			boost::shared_ptr<http_base> ptr;
			socket->get_socket(ptr);
			accept(handler, ptr->get_socket());
		}
		break;
		case HTTPS:
		{
			boost::shared_ptr<https_base> ptr;
			socket->get_socket(ptr);
			accept(handler, ptr->get_socket());
		}
		break;
		default:
			break;
		}
		return false;
	}

	template<protocal_type type>
	bool accept(boost::function<void(boost::system::error_code, boost::shared_ptr<b_net_socket_api>)> handler)
	{
		auto s_opt = make_server_socket(type);
		return accept(boost::bind(handler, _1, s_opt), s_opt);
	}



protected:
	template<typename Func, typename Protocol, typename SocketService>
	void accept(Func handler, boost::asio::basic_socket<Protocol, SocketService>& s)
	{
		base_core->accept(handler, s);
	}

	template<typename Protocol, typename SocketService>
	void accept(boost::asio::yield_context yc, boost::system::error_code& ec, boost::asio::basic_socket<Protocol, SocketService>& s)
	{
		accept(yc[ec], s);
	}


};


