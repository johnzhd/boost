#include "stdafx.h"
#include "net_api.hpp"

net_thread::net_thread()
{
	make_shared(io_opt);
	make_shared(signals_opt, *io_opt);
	make_shared(timer_opt, *io_opt);

	signals_opt->add(SIGINT);
	signals_opt->add(SIGTERM);
#if defined(SIGQUIT)
	signals_opt->add(SIGQUIT);
#endif // defined(SIGQUIT)
	signals_opt->async_wait(boost::bind(&net_thread::signal_handle, this, _1, _2));
}

net_thread::~net_thread()
{

}

boost::shared_ptr<boost::asio::io_service> net_thread::get_io()
{
	return io_opt;
}

bool net_thread::run()
{
	try
	{
		get_io()->run();
		return true;
	}
	catch (const boost::system::error_code& e)
	{
		boost_log(error, "io running boost err: %1%", e.message());
	}
	catch (const std::exception& e)
	{
		boost_log(error, "io running system err: %1%", e.what());
	}
	catch (...)
	{
		boost_log(error, "io running unkown err.");
	}
	return false;
}

void net_thread::stop()
{
	boost::system::error_code ec; // avoid the exception
	timer_opt->cancel(ec);
	get_io()->stop();
}

void net_thread::signal_handle(const boost::system::error_code & ec, int s)
{
	if (ec)
	{
		return;
	}
	stop();
}
