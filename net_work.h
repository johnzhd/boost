#pragma once


#include <boost/asio.hpp>
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


class net_work
{
public:
	net_work(void);
	~net_work(void);
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

	void accept(boost::asio::yield_context yield,
		boost::shared_ptr<boost::asio::ip::tcp::acceptor> ptr_,
		std::string address, std::string port)
	{
		boost::system::error_code ec;
		boost::asio::ip::tcp::endpoint local_endpoint_ = *dns(yield,ec,io_,address,port);
		ptr_->open(local_endpoint_.protocol());
		ptr_->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		ptr_->bind(local_endpoint_);
		ptr_->listen();

		auto s = new_socket();
		ptr_->async_accept(*s, yield[ec]);
	}


	boost::shared_ptr<boost::asio::ip::tcp::socket> new_socket()
	{
		assert(io_!=nullptr);
		return boost::make_shared<boost::asio::ip::tcp::socket>(*io_);
	}

	void connect(boost::asio::yield_context yield,
		boost::shared_ptr<boost::asio::ip::tcp::socket> s,
		std::string address, std::string port)
	{
		boost::system::error_code ec;
		s->async_connect(*dns(yield,ec,io_,address,port),yield[ec]);
	}

	template<typename T_BYTE_PTR>
	size_t send(boost::asio::yield_context yield,
		boost::shared_ptr<boost::asio::ip::tcp::socket> s,
		T_BYTE_PTR p, size_t size)
	{
		boost::system::error_code ec;
		return s->async_write_some(boost::asio::buffer(p,size), yield[ec] );
	}

	template<typename T_BYTE_CONTAINER>
	size_t send(boost::asio::yield_context yield,
		boost::shared_ptr<boost::asio::ip::tcp::socket> s,
		T_BYTE_CONTAINER& c)
	{
		boost::system::error_code ec;
		return s->async_write_some(boost::asio::buffer(c), yield[ec] );
	}

	template<typename T_BYTE_PTR>
	size_t recv(boost::asio::yield_context yield,
		boost::shared_ptr<boost::asio::ip::tcp::socket> s,
		T_BYTE_PTR p, size_t size)
	{
		boost::system::error_code ec;
		return s->async_read_some(boost::asio::buffer(p,size), yield[ec] );
	}

	template<typename T_BYTE_CONTAINER>
	size_t recv(boost::asio::yield_context yield,
		boost::shared_ptr<boost::asio::ip::tcp::socket> s,
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


};

