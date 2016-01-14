#pragma once


#include "socket_base.hpp"


namespace asio_tools
{
	template<typename Func>
	static boost::shared_ptr<boost::asio::ssl::context> make_context(Func& password_handler,
		boost::system::error_code& ec,
		boost::asio::ssl::context::method method = boost::asio::ssl::context::sslv23,
		std::string certificate_chain_file = "work.crt",
		std::string private_key_file = "work.key",
		std::string dh_file = std::string() /* = "dh512.pem" */
		)
	{
		// see create_server_pem.bat which in the create_pem folder
		auto ptr = boost::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
		if (!ptr)
		{
			return nullptr;
		}

		ptr->set_options(
			boost::asio::ssl::context::default_workarounds
			| boost::asio::ssl::context::no_sslv2
			| boost::asio::ssl::context::single_dh_use, ec);
		if (ec)
			return nullptr;

		ptr->set_password_callback(password_handler, ec);
		if (ec)
			return nullptr;

		ptr->use_certificate_chain_file(certificate_chain_file, ec);
		if (ec)
			return nullptr;

		ptr->use_private_key_file(private_key_file, boost::asio::ssl::context::pem, ec);
		if (ec)
			return nullptr;

		ptr->use_tmp_dh_file(dh_file, ec);
		if (ec)
			return nullptr;

		return ptr;
	}

	template<typename Func>
	static boost::shared_ptr<boost::asio::ssl::context> make_context(Func&& password_handler,
		boost::system::error_code& ec,
		boost::asio::ssl::context::method method = boost::asio::ssl::context::sslv23,
		std::string certificate_chain_file = "work.crt",
		std::string private_key_file = "work.key",
		std::string dh_file = std::string() /* = "dh512.pem" */
		)
	{
		// see create_server_pem.bat which in the create_pem folder
		auto ptr = boost::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);
		if (!ptr)
		{
			return nullptr;
		}

		ptr->set_options(
			boost::asio::ssl::context::default_workarounds
			| boost::asio::ssl::context::no_sslv2
			| boost::asio::ssl::context::single_dh_use, ec);
		if (ec)
			return nullptr;

		ptr->set_password_callback(password_handler, ec);
		if (ec)
			return nullptr;

		ptr->use_certificate_chain_file(certificate_chain_file, ec);
		if (ec)
			return nullptr;

		ptr->use_private_key_file(private_key_file, boost::asio::ssl::context::pem, ec);
		if (ec)
			return nullptr;

		ptr->use_tmp_dh_file(dh_file, ec);
		if (ec)
			return nullptr;

		return ptr;
	}


	// for client
	static boost::shared_ptr<socket_core> make_socket(boost::asio::io_service& io, protocal::protocal_type  type);
	// for server
	static boost::shared_ptr<boost::asio::ip::tcp::acceptor> make_acceptor(boost::asio::io_service& io);

	// for DNS
	static boost::shared_ptr<boost::asio::ip::tcp::resolver> make_resolver(boost::asio::io_service& io);

	template<typename Func>
	auto dns(Func&& handler,
		boost::asio::ip::tcp::resolver& resolver,
		std::string address,
		std::string port)
	{
		boost::asio::ip::tcp::resolver::query query(address, port);
		return resolver.async_resolve(query, handler);
	}


	template<typename Func>
	auto dns(Func& handler,
		boost::asio::ip::tcp::resolver& resolver,
		std::string address,
		std::string port)
	{
		boost::asio::ip::tcp::resolver::query query(address, port);
		return resolver.async_resolve(query, handler);
	}

	// for time
	static boost::shared_ptr<boost::asio::deadline_timer> make_timer(boost::asio::io_service& io);

	template<typename Func>
	static void wait_time(Func handler, boost::system::error_code& ec,
		boost::shared_ptr<boost::asio::deadline_timer> timer_opt, boost::posix_time::time_duration time)
	{
		assert(timer_opt);
		timer_opt->expires_from_now(time, ec);
		if (ec)
			return;
		timer_opt->async_wait(handler);
	}

	template<typename Func>
	static void wait_time(Func&& handler, boost::system::error_code& ec,
		boost::shared_ptr<boost::asio::deadline_timer> timer_opt, boost::posix_time::time_duration time)
	{
		assert(timer_opt);
		timer_opt->expires_from_now(time, ec);
		if (ec)
			return;
		timer_opt->async_wait(handler);
	}

	static boost::posix_time::time_duration convert_time(long h, long m, long s, size_t milliseconds, size_t microseconds);

	static boost::shared_ptr<boost::asio::signal_set> make_signal(boost::asio::io_service& io);

	template<typename Func>
	static bool start_signal(Func& handler, boost::system::error_code& ec, boost::shared_ptr<boost::asio::signal_set> s)
	{
		if (!s)
			return false;
		boost::system::error_code ec;
		s->add(SIGINT, ec);
		if (ec)
		{
			return false;
		}
		s->add(SIGTERM, ec);
		if (ec)
		{
			return false;
		}
#if defined(SIGQUIT)
		s->add(SIGQUIT, ec);
		if (ec)
		{
			return false;
		}
#endif // defined(SIGQUIT)
		s->async_wait(handler);
		return true;
	}
	template<typename Func>
	static bool start_signal(Func&& handler, boost::system::error_code& ec, boost::shared_ptr<boost::asio::signal_set> s)
	{
		if (!s)
			return false;
		boost::system::error_code ec;
		s->add(SIGINT, ec);
		if (ec)
		{
			return false;
		}
		s->add(SIGTERM, ec);
		if (ec)
		{
			return false;
		}
#if defined(SIGQUIT)
		s->add(SIGQUIT, ec);
		if (ec)
		{
			return false;
		}
#endif // defined(SIGQUIT)
		s->async_wait(handler);
		return true;
	}
};




