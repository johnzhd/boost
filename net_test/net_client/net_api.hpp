#pragma once

#include "b_net_socket_api.h"



class net_thread : public boost::enable_shared_from_this<net_thread>
{
public:
	boost::shared_ptr<boost::asio::signal_set> signals_opt;
	boost::shared_ptr<boost::asio::io_service> io_opt;
public:
	net_thread();
	~net_thread();
public:
	boost::shared_ptr<boost::asio::io_service> get_io();
	bool run();
	void stop();

	void signal_handle(const boost::system::error_code& ec, int s);


public:
	////// client work zone //////////
	inline void spawn_start(boost::function<void(boost::asio::yield_context yc)>& f)
	{
		boost::asio::spawn(*get_io(), f);
	}



public:
	boost::shared_ptr<b_net_socket_api> make_socket(protocal_type type);
	boost::shared_ptr<b_net_acceptor_api> make_acceptor();

	boost::shared_ptr<boost::asio::ip::tcp::resolver> make_resolver();
public:
	boost::shared_ptr<boost::asio::deadline_timer> make_timer();

	static void wait_time(boost::shared_ptr<boost::asio::deadline_timer> timer_opt, boost::posix_time::time_duration time,
		boost::asio::yield_context yc, boost::system::error_code& ec);

	static void cancel_time(boost::shared_ptr<boost::asio::deadline_timer> timer_opt, boost::system::error_code& ec);
};

boost::posix_time::time_duration make_time(long h, long m, long s, size_t milliseconds, size_t microseconds);


