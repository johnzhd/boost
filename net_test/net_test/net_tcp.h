#pragma once

#include "net_base.hpp"
#include "net_ssl_base.hpp"

#include <string>

template<typename T_SOCKET>
class net_tcp
{
public:
	net_tcp(boost::shared_ptr<boost::asio::io_service> io_):net_io(io_)
	{
		if ( net_io.expired())
			throw false;

		socket_base = boost::make_shared<T_SOCKET>(net_io.lock());
		if (socket_base.get() == nullptr)
			throw false;
	};
	~net_tcp(){};
public:
	
	boost::weak_ptr<boost::asio::io_service> net_io;

	//client
	boost::shared_ptr<T_SOCKET> socket_base;

	//server
	boost::shared_ptr<boost::asio::ip::tcp::acceptor> net_acceptor;

	// error
	boost::system::error_code ec;

public:
	bool init_ptr()
	{
		
	}

	T_SOCKET& get_base()
	{
		assert(socket_base);
		return *socket_base;
	}

public:
	
	// net work ...
	boost::shared_ptr<boost::asio::ip::tcp::acceptor> new_acceptor()
	{
		if ( net_io.expired() )
			return nullptr;

		
		return boost::make_shared<boost::asio::ip::tcp::acceptor>(*net_io.lock());
	}

	boost::shared_ptr<boost::asio::deadline_timer> new_timer(boost::posix_time::ptime t)
	{
		if ( net_io.expired() )
			return nullptr;

		return boost::make_shared<boost::asio::deadline_timer>(*net_io.lock(), t);

		// t->async_wait(boost::bind(timer_show,_1,t)); 

		//t->expires_at(t->expires_at() + boost::posix_time::seconds(5)); 
		//t->async_wait(boost::bind(timer_show, _1, t));
	}

	boost::asio::ip::tcp::resolver::iterator yield_dns(boost::asio::yield_context yield,
		std::string address,
		std::string port)
	{
		if ( net_io.expired() )
			return boost::asio::ip::tcp::resolver::iterator();

		boost::asio::ip::tcp::resolver resolver(*net_io.lock());
		boost::asio::ip::tcp::resolver::query query(address, port);
		return resolver.async_resolve(query,yield[ec]);
	}

	void accepter_bind( boost::asio::ip::tcp::endpoint local_endpoint_)
	{
		net_acceptor->open(local_endpoint_.protocol(),ec);
		net_acceptor->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true),ec);
		net_acceptor->bind(local_endpoint_,ec);
		net_acceptor->listen(boost::asio::socket_base::max_connections,ec);
	}

	template<typename SOCKET_PTR, typename BYTE_CONTAINER>
	size_t yield_send(boost::asio::yield_context yield,
		BYTE_CONTAINER& buff)
	{
		return get_base().get_socket()->async_write_some(boost::asio::buffer(buff), yield[ec] );
	}

	template<typename BYTE_CONTAINER>
	size_t yield_send_hold(boost::asio::yield_context yield,
		BYTE_CONTAINER& buff)
	{
		return boost::asio::async_write(*get_base().get_socket(),
			boost::asio::buffer(buff),
			boost::asio::transfer_at_least(buff.size()), yield[ec] );
	}

	template<typename BYTE_CONTAINER>
	size_t yield_recv(boost::asio::yield_context yield,
		BYTE_CONTAINER& buff)
	{
		return get_base().get_socket()->async_read_some(boost::asio::buffer(buff), yield[ec] );
	}



public:
	//string
};


