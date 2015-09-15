#include "stdafx.h"
#include "net_api.hpp"

net_thread::net_thread()
{
	make_shared(io_opt);
	make_shared(signals_opt, *io_opt);

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

boost::shared_ptr<b_net_socket_api> net_thread::make_socket(protocal_type type)
{
	boost::shared_ptr<b_net_socket_api> opt;
	return make_shared(opt, type, io_opt);
}


boost::shared_ptr<b_net_acceptor_api> net_thread::make_acceptor()
{
	boost::shared_ptr<b_net_acceptor_api> opt;
	return make_shared(opt, io_opt);
}

boost::shared_ptr<boost::asio::ip::tcp::resolver> net_thread::make_resolver()
{
	return boost::make_shared<boost::asio::ip::tcp::resolver>(*io_opt);
}

boost::shared_ptr<boost::asio::deadline_timer> net_thread::make_timer()
{
	return boost::make_shared<boost::asio::deadline_timer>(*io_opt);
}

void net_thread::wait_time(boost::shared_ptr<boost::asio::deadline_timer> timer_opt, boost::posix_time::time_duration time, boost::asio::yield_context yc, boost::system::error_code & ec)
{
	assert(timer_opt);
	timer_opt->expires_from_now(time, ec);
	if (ec)
		return;
	timer_opt->async_wait(yc[ec]);
}

void net_thread::cancel_time(boost::shared_ptr<boost::asio::deadline_timer> timer_opt, boost::system::error_code & ec)
{
	assert(timer_opt);
	timer_opt->cancel(ec);
}

boost::posix_time::time_duration make_time(long h, long m, long s, size_t milliseconds, size_t microseconds)
{
	return boost::posix_time::hours(h)
		+ boost::posix_time::minutes(m)
		+ boost::posix_time::seconds(s)
		+ boost::posix_time::milliseconds(milliseconds)
		+ boost::posix_time::microseconds(microseconds);
}
