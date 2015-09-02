#pragma once

#include "b_net_socket_api.h"


class net_thread : public boost::enable_shared_from_this<net_thread>
{
public:
	boost::shared_ptr<boost::asio::deadline_timer> timer_opt;
	boost::shared_ptr<boost::asio::signal_set> signals_opt;
	boost::shared_ptr<boost::asio::io_service> io_opt;
public:
	net_thread()
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
	~net_thread() {}
public:
	inline boost::shared_ptr<boost::asio::io_service> get_io()
	{
		return io_opt;
	}
	bool run()
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
	void stop()
	{
		boost::system::error_code ec; // avoid the exception
		timer_opt->cancel(ec);
		get_io()->stop();
	}

	void signal_handle(const boost::system::error_code& ec, int s)
	{
		if (ec)
		{
			return;
		}
		stop();
	}


public:
	////// client work zone //////////
	template<typename Func>
	void spawn_start(Func& f)
	{
		boost::asio::spawn(*get_io(), f);
	}
};

class net_work
{
public:
	net_work(boost::shared_ptr<net_thread> t);
	~net_work();
protected:
	boost::shared_ptr<net_thread> thread_opt;
	boost::asio::io_service& get_io();
public:
	void stop_signal(int s);
public:
	void demo_loop(boost::asio::yield_context yc);
};

