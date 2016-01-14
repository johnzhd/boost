#include "stdafx.h"
#include "socket_work.hpp"

time_socket::time_socket(boost::asio::io_service & io, protocal::protocal_type t, bool need_extern)
{
	make_shared(core, io, t, need_extern);
}

time_socket::time_socket(boost::asio::io_service & io, boost::asio::ssl::context & context, bool need_extern)
{
	make_shared(core, io, context, need_extern);
}

boost::shared_ptr<socket_core> time_socket::get_socket()
{
	return core;
}

boost::shared_ptr<socket_core> time_socket::start_timer(size_t second)
{
	boost::system::error_code ec;
	if (!timer_core)
	{
		make_shared(timer_core, core->get_io());
	}
	timer_core->expires_from_now(boost::posix_time::seconds(second), ec);
	if (ec)
		return boost::shared_ptr<socket_core>();
	timer_core->async_wait(boost::bind(&time_socket::timer_handler, shared_from_this(), _1));
	return core;
}

void time_socket::cancel_timer()
{
	if (timer_core)
		timer_core->cancel();
}

void time_socket::timer_handler(boost::system::error_code ec)
{
	if (ec)
		return;
	if (core)
		core->cancel();
}
