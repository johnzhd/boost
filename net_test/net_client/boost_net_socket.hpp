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


typedef enum protocal_type
{
	HTTP = 0,  // for http_base
	HTTPS,     // for https_base
}protocal_type;

class http_base
{
public:
	http_base(boost::asio::io_service& io);
	~http_base(void);
public:
	boost::asio::ip::tcp::socket s_opt;
public:
	bool connect(boost::asio::yield_context& yc, boost::system::error_code& ec, boost::asio::ip::tcp::endpoint ep);

	void cancel(void);


	template<typename ...ARGS>
	size_t send(boost::asio::yield_context yc,
		boost::system::error_code& ec,
		ARGS&...args)
	{
		return s_opt.async_write_some(boost::asio::buffer(args...), yc[ec]);
	}

	template<typename ...ARGS>
	size_t send_at_least(boost::asio::yield_context yc,
		boost::system::error_code& ec,
		size_t at_least_size,
		ARGS& ...args)
	{
		return boost::asio::async_write(s_opt, boost::asio::buffer(args...), boost::asio::transfer_at_least(at_least_size), yc[ec]);
		//return s_opt.async_write_some(s_opt, boost::asio::buffer(args...), yc[ec]);

	}

	template<typename ...ARGS>
	size_t recv(boost::asio::yield_context yc,
		boost::system::error_code& ec,
		ARGS&...args)
	{
		return s_opt.async_read_some(boost::asio::buffer(args...), yc[ec]);
	}

public:
	bool get_extern_info(std::vector<unsigned char>& ret);
};




class https_base
{
public:
	https_base(boost::asio::io_service& io);
	~https_base(void);
public:
	boost::asio::ssl::stream<boost::asio::ip::tcp::socket> s_opt;
public:
	bool connect(boost::asio::yield_context& yc, boost::system::error_code& ec, boost::asio::ip::tcp::endpoint ep);

	void cancel(void);

	template<typename ...ARGS>
	size_t send(boost::asio::yield_context yc,
		boost::system::error_code& ec,
		ARGS&...args)
	{
		return s_opt.async_write_some(boost::asio::buffer(args...), yc[ec]);
	}

	template<typename ...ARGS>
	size_t send_at_least(boost::asio::yield_context yc,
		boost::system::error_code& ec,
		size_t at_least_size,
		ARGS& ...args)
	{
		return boost::asio::async_write(s_opt, boost::asio::buffer(args...), boost::asio::transfer_at_least(at_least_size), yc[ec]);
		//return s_opt.async_write_some(boost::asio::buffer(args...), yc[ec]);
	}

	template<typename ...ARGS>
	size_t recv(boost::asio::yield_context yc,
		boost::system::error_code& ec,
		ARGS&...args)
	{
		return s_opt.async_read_some(boost::asio::buffer(args...), yc[ec]);
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



