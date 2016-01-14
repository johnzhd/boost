#pragma once

#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "socket_base.hpp"

class time_socket : public boost::enable_shared_from_this<time_socket>
{
public:
	time_socket(boost::asio::io_service& io, protocal::protocal_type t, bool need_extern = false);

	time_socket(boost::asio::io_service& io, boost::asio::ssl::context& context, bool need_extern = false);
public:
	boost::shared_ptr<boost::asio::deadline_timer> timer_core;
public:
	boost::shared_ptr<socket_core> core;
public:
	boost::shared_ptr<socket_core> get_socket();
	boost::shared_ptr<socket_core> start_timer(size_t second);
	void cancel_timer();
protected:
	void timer_handler(boost::system::error_code ec);
};


