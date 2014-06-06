#pragma once


#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/spawn.hpp>

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <boost/date_time/posix_time/posix_time.hpp> 
#include <boost/function.hpp>
#include <assert.h>

template<const char* T_PASSWORD = "test">
class net_ssl_base
{
public:
	net_ssl_base(void)
	{
	};
	~net_ssl_base(void)
	{
	};
public:
	boost::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> socket_;

	boost::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> get_socket()
	{
		return socket_;
	}

protected:
	static std::string get_password()
	{
		return T_PASSWORD;
	}
public:
	static boost::shared_ptr<boost::asio::ssl::context> new_context_server(boost::system::error_code& ec)
	{
		auto ptr = boost::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
		if ( !ptr )
		{
			return nullptr;
		}

		ptr->set_options(
			boost::asio::ssl::context::default_workarounds
			| boost::asio::ssl::context::no_sslv2
			| boost::asio::ssl::context::single_dh_use, ec);
		if ( ec )
			return nullptr;

		ptr->set_password_callback(&net_ssl_base::get_password,ec);
		if ( ec )
			return nullptr;

		ptr->use_certificate_chain_file("server.pem",ec);
		if ( ec )
			return nullptr;

		ptr->use_private_key_file("server.pem", boost::asio::ssl::context::pem,ec);
		if ( ec )
			return nullptr;

		ptr->use_tmp_dh_file("dh512.pem",ec);
		if ( ec )
			return nullptr;

		return ptr;
	}

	static boost::shared_ptr<boost::asio::ssl::context> new_context_client(boost::system::error_code& ec)
	{
		auto ptr = boost::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
		if ( !ptr )
		{
			return nullptr;
		}
		ptr->load_verify_file( "ca.pem",err);

		return ptr;
	}
	
	static boost::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> new_socket(
		boost::shared_ptr<boost::asio::io_service> io_,
		boost::asio::ssl::context& ctx)
	{
		assert(io_);
		return boost::make_shared<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>(*io_,ctx);
	}

	static void yield_accept(boost::asio::yield_context yield,
		boost::system::error_code &ec,
		boost::shared_ptr<boost::asio::ip::tcp::acceptor> ptr_,
		boost::asio::ip::tcp::endpoint local_endpoint_, boost::asio::ssl::context& ctx)
	{
		auto s = new_socket(ctx);
		ptr_->async_accept(s->lowest_layer(), yield[ec]);
		if ( ec )
			return ;

		s->async_handshake(boost::asio::ssl::stream_base::server,yield[ec]);
		if ( ec )
			return ;
	}



protected:
	static bool verify_certificate(bool preverified,
		boost::asio::ssl::verify_context& ctx)
	{
		// The verify callback can be used to check whether the certificate that is
		// being presented is valid for the peer. For example, RFC 2818 describes
		// the steps involved in doing this for HTTPS. Consult the OpenSSL
		// documentation for more details. Note that the callback is called once
		// for each certificate in the certificate chain, starting from the root
		// certificate authority.

		// In this example we will simply print the certificate's subject name.
		//char subject_name[256];
		//X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
		//X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
		//std::cout << "Verifying " << subject_name << "\n";

		//return preverified;
		return true;
	}
public:

	static void yield_connect(boost::asio::yield_context yield,
		boost::system::error_code &ec,
		boost::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> s,
		boost::asio::ip::tcp::endpoint local_endpoint_)
	{
		if ( ec )
			return ;

		
		s->set_verify_mode(boost::asio::ssl::verify_peer,ec);
		s->set_verify_callback(	&net_ssl_base::verify_certificate, ec);

		boost::asio::async_connect(s->lowest_layer(), local_endpoint_, yield[ec]);
		if ( ec )
		{
			return ;
		}

		s->async_handshake(boost::asio::ssl::stream_base::client, yield[ec]);
		if ( ec )
		{
			return ;
		}
	}

};

