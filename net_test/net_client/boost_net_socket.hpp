#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/spawn.hpp>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/condition.hpp>

#include <boost/date_time/posix_time/posix_time.hpp> 

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/function.hpp>

#include <assert.h>


boost::asio::ip::tcp::endpoint dns(boost::asio::yield_context yc,
	boost::system::error_code& ec,
	boost::asio::ip::tcp::resolver& resolver,
	std::string address,
	std::string port);

template<typename Func>
auto dns(Func handler,
	boost::asio::ip::tcp::resolver& resolver,
	std::string address,
	std::string port)
{
	boost::asio::ip::tcp::resolver::query query(address, port);
	return resolver.async_resolve(query, handler);
}



class http_base : public boost::noncopyable
{
public:
	typedef boost::asio::ip::tcp::socket lowsest_layer_socket_type;
public:
	http_base(boost::asio::io_service& io);
	~http_base(void);
public:
	boost::asio::ip::tcp::socket s_opt;
	lowsest_layer_socket_type& get_socket();
public:
	bool connect(boost::asio::yield_context yc, boost::system::error_code& ec, boost::asio::ip::tcp::endpoint ep);

	template<typename Func>
	void connect(Func handler, boost::asio::ip::tcp::endpoint ep)
	{
		s_opt.async_connect(ep, handler);
	}

	template<typename Func>
	void handshake(Func handler, boost::asio::ssl::stream_base::handshake_type t = boost::asio::ssl::stream_base::client)
	{
		// nothing
	}

	void cancel(void);

	template<typename Func, typename ...ARGS>
	auto send(Func handler, ARGS&...args)
	{
		return s_opt.async_write_some(boost::asio::buffer(args...), handler);
	}

	template<typename ...ARGS>
	size_t send(boost::asio::yield_context yc,
		boost::system::error_code& ec,
		ARGS&...args)
	{
		return send(yc[ec], args...);
	}

	template<typename Func, typename ...ARGS>
	size_t send_at_least(Func handler,
		size_t at_least_size,
		ARGS& ...args)
	{
		return boost::asio::async_write(s_opt, boost::asio::buffer(args...), boost::asio::transfer_at_least(at_least_size), handler);
		//return s_opt.async_write_some(s_opt, boost::asio::buffer(args...), yc[ec]);
	}

	template<typename ...ARGS>
	size_t send_at_least(boost::asio::yield_context yc,
		boost::system::error_code& ec,
		size_t at_least_size,
		ARGS& ...args)
	{
		return send_at_least(yc[ec], at_least_size, args...);
		//return s_opt.async_write_some(s_opt, boost::asio::buffer(args...), yc[ec]);
	}

	template<typename Func, typename ...ARGS>
	size_t recv(Func handler,
		ARGS&...args)
	{
		return s_opt.async_read_some(boost::asio::buffer(args...), handler);
	}

	template<typename ...ARGS>
	size_t recv(boost::asio::yield_context yc,
		boost::system::error_code& ec,
		ARGS&...args)
	{
		return recv(yc[ec], args...);
	}

public:
	bool get_extern_info(std::vector<unsigned char>& ret);
};




class https_base : public boost::noncopyable
{
public:
	typedef boost::asio::ssl::stream<boost::asio::ip::tcp::socket>::lowest_layer_type lowsest_layer_socket_type;
public:
	https_base(boost::asio::io_service& io);
	https_base(boost::asio::io_service& io, boost::shared_ptr<boost::asio::ssl::context> ctx);

	~https_base(void);
public:
	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> s_opt;

	lowsest_layer_socket_type& get_socket()
	{
		return s_opt.lowest_layer();
	}
public:

	template<typename Func>
	void connect(Func handler, boost::asio::ip::tcp::endpoint ep)
	{
		s_opt.set_verify_mode(boost::asio::ssl::verify_peer);
		s_opt.set_verify_callback(boost::bind(&https_base::verify_certificate, this, _1, _2));
		s_opt.lowest_layer().async_connect(ep,handler);
	}

	template<typename Func>
	void handshake(Func handler, boost::asio::ssl::stream_base::handshake_type t = boost::asio::ssl::stream_base::client)
	{
		s_opt.async_handshake(t, handler);
	}

	bool connect(boost::asio::yield_context yc, boost::system::error_code& ec, boost::asio::ip::tcp::endpoint ep);

	void cancel(void);

	template<typename Func, typename ...ARGS>
	size_t send(Func handler,
		ARGS&...args)
	{
		return s_opt.async_write_some(boost::asio::buffer(args...), handler);
	}

	template<typename ...ARGS>
	size_t send(boost::asio::yield_context yc,
		boost::system::error_code& ec,
		ARGS&...args)
	{
		return send(yc[ec], args...);
	}

	template<typename Func, typename ...ARGS>
	size_t send_at_least(Func handler,
		size_t at_least_size,
		ARGS& ...args)
	{
		return boost::asio::async_write(s_opt, boost::asio::buffer(args...), boost::asio::transfer_at_least(at_least_size), handler);
		//return s_opt.async_write_some(boost::asio::buffer(args...), yc[ec]);
	}

	template<typename ...ARGS>
	size_t send_at_least(boost::asio::yield_context yc,
		boost::system::error_code& ec,
		size_t at_least_size,
		ARGS& ...args)
	{
		return send_at_least(yc[ec], at_least_size, args...);
		//return s_opt.async_write_some(boost::asio::buffer(args...), yc[ec]);
	}


	template<typename Func, typename ...ARGS>
	size_t recv(Func handler,
		ARGS&...args)
	{
		return s_opt.async_read_some(boost::asio::buffer(args...), handler);
	}

	template<typename ...ARGS>
	size_t recv(boost::asio::yield_context yc,
		boost::system::error_code& ec,
		ARGS&...args)
	{
		return recv(yc[ec], args...);
	}



public:
	bool verify_certificate(bool preverified,
		boost::asio::ssl::verify_context& ctx)
	{
		// The verify callback can be used to check whether the certificate that is
		// being presented is valid for the peer. For example, RFC 2818 describes
		// the steps involved in doing this for HTTPS. Consult the OpenSSL
		// documentation for more details. Note that the callback is called once
		// for each certificate in the certificate chain, starting from the root
		// certificate authority.

		// In this example we will simply print the certificate's subject name.
#ifdef _DEBUG
		char subject_name[256];
		std::stringstream ss;
		std::string str;
		X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
		X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);

		//std::cout << "Verifying " << subject_name << "\n";

		ss << "name: " << subject_name << "\nversion: " << X509_get_version(cert);
		str = ss.str();

		std::copy(str.begin(), str.end(), std::back_inserter(extern_info));

		//X509_NAME_oneline(X509_get_issuer_name(cert), subject_name, 256);
		//std::cout << "issuer_name " << subject_name << "\n";

		//auto p = X509_get1_email(cert);
		//auto n = X509_get_serialNumber(cert);
		//auto v = X509_get_version(cert);
		//auto k = X509_get_pubkey(cert);
#endif
		//return preverified;
		return true;
	}
public:
	bool get_extern_info(std::vector<unsigned char>& ret);
protected:
	std::vector<unsigned char> extern_info;
};


//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////// server ///////////////////////////////////////////////////////////

class http_server_base
{
public:
	http_server_base(boost::asio::io_service& io);
	~http_server_base();
protected:
	boost::shared_ptr<boost::asio::ip::tcp::resolver> resolver_opt;
	boost::shared_ptr<boost::asio::ip::tcp::acceptor> accept_opt;
	boost::shared_ptr<boost::asio::ssl::context> ctx_opt;
public:
	boost::asio::io_service& get_io();
public:
	boost::function<void(boost::asio::ip::tcp::socket s)> cbfunc_accept_one;

	void bind_listen(boost::system::error_code& ec, boost::asio::ip::tcp::endpoint ep);

	template<typename Func, typename Protocol, typename SocketService>
	void accept(Func handler, boost::asio::basic_socket<Protocol, SocketService>& s)
	{
		accept_opt->async_accept(s, handler);
	}

	template<typename Protocol, typename SocketService>
	void accept(boost::asio::yield_context yc, boost::system::error_code& ec, boost::asio::basic_socket<Protocol, SocketService>& s)
	{
		accept(yc[ec], s);
	}

	void cancel()
	{
		if(accept_opt)
			accept_opt->cancel();
	}
public:
	static std::string get_password(std::size_t max_length, boost::asio::ssl::context_base::password_purpose purpose);
public:
	boost::shared_ptr<boost::asio::ssl::context> new_context(boost::system::error_code& ec);
};





