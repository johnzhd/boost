#pragma once

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>


class asio_io_service
{
public:
	asio_io_service() {};
	~asio_io_service() {};
public:
	boost::asio::io_service io_core;
public:
	boost::asio::io_service& get_io() { return io_core };
public:
	template<typename Func>
	void spawn(Func& handler)
	{
		boost::asio::spawn(get_io(), std::move(handler));
	}

	template<typename Func>
	void spawn(Func&& handler)
	{
		boost::asio::spawn(get_io(), handler);
	}

	template<typename Func>
	void post(Func& handler)
	{
		get_io().post(std::move(handler));
	}

	template<typename Func>
	void post(Func&& handler)
	{
		get_io().post(handler);
	}

	bool run()
	{
		try
		{
			get_io().run();
			return true;
		}
		catch (boost::system::error_code ec)
		{

		}
		catch (std::exception e)
		{

		}
		catch (...)
		{

		}
		return false;
	}

	void stop()
	{
		get_io().stop();
	}

	bool is_stop()
	{
		return get_io().stopped();
	}
};



