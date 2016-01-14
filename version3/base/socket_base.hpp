#pragma once

#include <vector>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/spawn.hpp>

#include "net_define.hpp"


/////
#include <string>
#include <sstream>
#include "memory_tools.hpp"


class socket_core
{
protected:
	boost::shared_ptr<boost::asio::ip::tcp::socket> tcp_opt;
	boost::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> tcp_ssl_opt;
	protocal::protocal_type type;
	boost::shared_ptr<std::vector<unsigned char>> extern_info;
public:
	socket_core(boost::asio::io_service& io, protocal::protocal_type t, bool need_extern = false);

	socket_core(boost::asio::io_service& io, boost::asio::ssl::context& context, bool need_extern = false);
public:
	boost::asio::basic_socket<boost::asio::ip::tcp, boost::asio::stream_socket_service<boost::asio::ip::tcp>>& get_socket();

	boost::asio::io_service& get_io();

	void cancel(void);
protected:
	bool verify_certificate(bool preverified,
		boost::asio::ssl::verify_context& ctx);
};

// move out of class to avoid resize
template<typename Core_Ptr, typename Func>
void connect(Core_Ptr core, Func& handler, boost::asio::ip::tcp::endpoint ep)
{
	if (core->type == protocal::TCP_SSL)
	{
		boost::system::error_code ec;
		core->tcp_ssl_opt->set_verify_mode(boost::asio::ssl::verify_peer, ec);
		if (!extern_info)
			core->tcp_ssl_opt->set_verify_callback([](bool preverified,
				boost::asio::ssl::verify_context& ctx)->bool {return true; }, ec);
		else
			core->tcp_ssl_opt->set_verify_callback(boost::bind(&socket_core::verify_certificate, core, _1, _2), ec);
	}
	core->get_socket().async_connect(ep, handler);
}

template<typename Core_Ptr, typename Func>
void connect(Core_Ptr core, Func&& handler, boost::asio::ip::tcp::endpoint ep)
{
	if (core->type == protocal::TCP_SSL)
	{
		boost::system::error_code ec;
		core->tcp_ssl_opt->set_verify_mode(boost::asio::ssl::verify_peer, ec);
		if (!extern_info)
			core->tcp_ssl_opt->set_verify_callback([](bool preverified,
				boost::asio::ssl::verify_context& ctx)->bool {return true; }, ec);
		else
			core->tcp_ssl_opt->set_verify_callback(boost::bind(&socket_core::verify_certificate, core, _1, _2), ec);
	}
	core->get_socket().async_connect(ep, handler);
}

template<typename Core_Ptr, typename Func>
void handshake(Core_Ptr core, Func& handler, boost::asio::ssl::stream_base::handshake_type t = boost::asio::ssl::stream_base::client)
{
	// nothing for TCP

	if (core->type == protocal::TCP_SSL)
	{
		core->tcp_ssl_opt->async_handshake(t, handler);
	}
}


template<typename Core_Ptr, typename Func>
void handshake(Core_Ptr core, Func&& handler, boost::asio::ssl::stream_base::handshake_type t = boost::asio::ssl::stream_base::client)
{
	// nothing for TCP

	if (core->type == protocal::TCP_SSL)
	{
		core->tcp_ssl_opt->async_handshake(t, handler);
	}
}


template<typename Core_Ptr, typename Func, typename ...ARGS>
auto send(Core_Ptr core, Func& handler, ARGS&...args)
{
	return core->get_socket().async_write_some(boost::asio::buffer(args...), handler);
}

template<typename Core_Ptr, typename Func, typename ...ARGS>
auto send(Core_Ptr core, Func&& handler, ARGS&...args)
{
	return core->get_socket().async_write_some(boost::asio::buffer(args...), handler);
}

template<typename Core_Ptr, typename Func, typename ...ARGS>
size_t send_at_least(Core_Ptr core, Func& handler,
	size_t at_least_size,
	ARGS& ...args)
{
	return boost::asio::async_write(core->get_socket(), boost::asio::buffer(args...),
		boost::asio::transfer_at_least(at_least_size), handler);
}


template<typename Core_Ptr, typename Func, typename ...ARGS>
size_t send_at_least(Core_Ptr core, Func&& handler,
	size_t at_least_size,
	ARGS& ...args)
{
	return boost::asio::async_write(core->get_socket(), boost::asio::buffer(args...),
		boost::asio::transfer_at_least(at_least_size), handler);
}

template<typename Core_Ptr, typename Func, typename ...ARGS>
size_t recv(Core_Ptr core, Func& handler,
	ARGS&...args)
{
	return core->get_socket().async_read_some(boost::asio::buffer(args...), handler);
}

template<typename Core_Ptr, typename Func, typename ...ARGS>
size_t recv(Core_Ptr core, Func&& handler,
	ARGS&...args)
{
	return core->get_socket().async_read_some(boost::asio::buffer(args...), handler);
}



class http_server_base
{
public:
	http_server_base(boost::asio::io_service& io);
	~http_server_base();
protected:
	boost::shared_ptr<boost::asio::ip::tcp::acceptor> accept_opt;
	boost::shared_ptr<boost::asio::ssl::context> ctx_opt;
public:
	boost::asio::io_service& get_io();
public:
	void bind_listen(boost::system::error_code& ec, boost::asio::ip::tcp::endpoint ep);

	void cancel();
public:
	static std::string get_password(std::size_t max_length, boost::asio::ssl::context_base::password_purpose purpose);
public:

};

template<typename Core_Ptr, typename Func, typename Protocol, typename SocketService>
void accept(Core_Ptr core, Func& handler, boost::asio::basic_socket<Protocol, SocketService>& s)
{
	core->accept_opt->async_accept(s, handler);
}

template<typename Core_Ptr, typename Func, typename Protocol, typename SocketService>
void accept(Core_Ptr core, Func&& handler, boost::asio::basic_socket<Protocol, SocketService>& s)
{
	core->accept_opt->async_accept(s, handler);
}
















