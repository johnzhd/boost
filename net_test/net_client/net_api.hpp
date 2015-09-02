#pragma once

#include "b_net_socket_api.h"


class net_thread : public boost::enable_shared_from_this<net_thread>
{
public:
	boost::shared_ptr<boost::asio::deadline_timer> timer_opt;
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
	template<typename Func>
	void spawn_start(Func& f)
	{
		boost::asio::spawn(*get_io(), f);
	}
};

