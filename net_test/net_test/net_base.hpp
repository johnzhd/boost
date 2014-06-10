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

template<char T = 1>
class net_base
{
public:
	net_base(boost::shared_ptr<boost::asio::io_service> net_io)
		:socket_(new_socket(net_io))
	{
	};
	~net_base(void)
	{
		socket_ = nullptr;
	}
protected:
	boost::shared_ptr<boost::asio::ip::tcp::socket> socket_;
public:
	boost::shared_ptr<boost::asio::ip::tcp::socket> get_socket()
	{
		return socket_;
	};

public:
	static boost::shared_ptr<boost::asio::ip::tcp::socket> new_socket(boost::shared_ptr<boost::asio::io_service> io_)
	{
		assert(io_);
		return boost::make_shared<boost::asio::ip::tcp::socket>(*io_);
	}

	static void yield_accept(boost::asio::yield_context yield,
		boost::system::error_code &ec,
		boost::shared_ptr<boost::asio::ip::tcp::acceptor> ptr_)
	{
		auto s = new_socket();
		ptr_->async_accept(*s, yield[ec]);
	}

	static void yield_connect(boost::asio::yield_context yield,
		boost::system::error_code &ec,
		boost::shared_ptr<boost::asio::ip::tcp::socket> s,
		boost::asio::ip::tcp::endpoint local_endpoint_)
	{
		s->async_connect(local_endpoint_,yield[ec]);
	}
};

