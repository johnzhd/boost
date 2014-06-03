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

class net_ssl_base
{
public:
	net_ssl_base(void);
	~net_ssl_base(void);
public:
	boost::shared_ptr<boost::asio::io_service> io_;
	boost::shared_ptr<boost::asio::io_service> get_service();

	
	template<typename T_FUNC>
	void pre( T_FUNC fc )
	{
		assert( io_ != nullptr);
		// fc = boost::bind(func_point,_1);
		if ( fc )
			boost::asio::spawn(*io_,fc);
	}

	template<typename T_FUNC>
	void run( T_FUNC fc)
	{
		assert( io_ != nullptr );
		if ( fc )
			fc();
		try
		{
			io_->run();
		}
		catch(...)
		{
		};
	};

public:
	
	boost::asio::ip::tcp::resolver::iterator dns(boost::asio::yield_context yield,
		boost::system::error_code& ec,
		boost::shared_ptr<boost::asio::io_service> server_,
		std::string address,
		std::string port)
	{
		boost::asio::ip::tcp::resolver resolver(*server_);
		boost::asio::ip::tcp::resolver::query query(address, port);

		boost::asio::ip::tcp::resolver::iterator resolver_it;

		return resolver.async_resolve(query,yield[ec]);
	}

	boost::shared_ptr<boost::asio::ip::tcp::acceptor> new_acceptor()
	{
		assert(io_!=nullptr);
		return boost::make_shared<boost::asio::ip::tcp::acceptor>(*io_);
	}

protected:
	std::string get_password() const
	{
		return "test";
	}
public:
	boost::shared_ptr<boost::asio::ssl::context> new_context(boost::system::error_code& ec)
	{
		assert(io_!=nullptr);
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

		ptr->set_password_callback(boost::bind(&net_ssl_base::get_password, this),ec);
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


	void accept(boost::asio::yield_context yield,
		boost::shared_ptr<boost::asio::ip::tcp::acceptor> ptr_,
		std::string address, std::string port, boost::asio::ssl::context& ctx)
	{
		boost::system::error_code ec;
		boost::asio::ip::tcp::endpoint local_endpoint_ = *dns(yield,ec,io_,address,port);
		ptr_->open(local_endpoint_.protocol());
		ptr_->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		ptr_->bind(local_endpoint_);
		ptr_->listen();

		auto s = new_socket(ctx);
		ptr_->async_accept(s->lowest_layer(), yield[ec]);
		if ( ec )
			return ;

		s->async_handshake(boost::asio::ssl::stream_base::server,yield[ec]);
		if ( ec )
			return ;
	}


	boost::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> new_socket(boost::asio::ssl::context& ctx )
	{
		assert(io_!=nullptr);
		boost::system::error_code ec;
		auto s = boost::make_shared<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>>(*io_,ctx);
		return s;
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
		//char subject_name[256];
		//X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
		//X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
		//std::cout << "Verifying " << subject_name << "\n";

		//return preverified;
		return true;
	}
public:

	void connect(boost::asio::yield_context yield,
		boost::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> s,
		std::string address, std::string port)
	{
		boost::system::error_code ec;
		auto it = dns(yield,ec,io_,address,port);
		if ( ec )
			return ;

		
		s->set_verify_mode(boost::asio::ssl::verify_peer,ec);
		s->set_verify_callback(	boost::bind(&net_ssl_base::verify_certificate, this, _1, _2), ec);

		boost::asio::async_connect(s->lowest_layer(), it, yield[ec]);

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

	template<typename T_BYTE_PTR>
	size_t send(boost::asio::yield_context yield,
		boost::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> s,
		T_BYTE_PTR p, size_t size)
	{
		boost::system::error_code ec;
		return s->async_write_some(boost::asio::buffer(p,size), yield[ec] );
	}

	template<typename T_BYTE_CONTAINER>
	size_t send(boost::asio::yield_context yield,
		boost::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> s,
		T_BYTE_CONTAINER& c)
	{
		boost::system::error_code ec;
		return s->async_write_some(boost::asio::buffer(c), yield[ec] );
	}

	template<typename T_BYTE_PTR>
	size_t recv(boost::asio::yield_context yield,
		boost::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> s,
		T_BYTE_PTR p, size_t size)
	{
		boost::system::error_code ec;
		return s->async_read_some(boost::asio::buffer(p,size), yield[ec] );
	}

	template<typename T_BYTE_CONTAINER>
	size_t recv(boost::asio::yield_context yield,
		boost::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> s,
		T_BYTE_CONTAINER& c)
	{
		boost::system::error_code ec;
		return s->async_read_some(boost::asio::buffer(c), yield[ec] );
	}



public:

	boost::shared_ptr<boost::asio::deadline_timer> new_timer(boost::posix_time::ptime t)
	{
		assert(io_!=nullptr);
		return boost::make_shared<boost::asio::deadline_timer>(*io_, t);

		// t->async_wait(boost::bind(timer_show,_1,t)); 

		//t->expires_at(t->expires_at() + boost::posix_time::seconds(5)); 
		//t->async_wait(boost::bind(timer_show, _1, t));
	}


protected:
	boost::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> socket_;
	boost::asio::ip::tcp::resolver::iterator it;

};

