#include "stdafx.h"
#include "asio_tools.hpp"

// for client

boost::shared_ptr<socket_core> asio_tools::make_socket(boost::asio::io_service & io, protocal::protocal_type type)
{
	boost::shared_ptr<socket_core> ptr;
	return make_shared(ptr, io, type);
}

// for server

boost::shared_ptr<boost::asio::ip::tcp::acceptor> asio_tools::make_acceptor(boost::asio::io_service & io)
{
	boost::shared_ptr<boost::asio::ip::tcp::acceptor> ptr;
	return make_shared(ptr, io);
}

// for DNS

boost::shared_ptr<boost::asio::ip::tcp::resolver> asio_tools::make_resolver(boost::asio::io_service & io)
{
	boost::shared_ptr<boost::asio::ip::tcp::resolver> ptr;
	return make_shared(ptr, io);
}

// for time

boost::shared_ptr<boost::asio::deadline_timer> asio_tools::make_timer(boost::asio::io_service & io)
{
	boost::shared_ptr<boost::asio::deadline_timer> ptr;
	return make_shared(ptr, io);
}

boost::posix_time::time_duration asio_tools::convert_time(long h, long m, long s, size_t milliseconds, size_t microseconds)
{
	return boost::posix_time::hours(h)
		+ boost::posix_time::minutes(m)
		+ boost::posix_time::seconds(s)
		+ boost::posix_time::milliseconds(milliseconds)
		+ boost::posix_time::microseconds(microseconds);
}

boost::shared_ptr<boost::asio::signal_set> asio_tools::make_signal(boost::asio::io_service & io)
{
	boost::shared_ptr<boost::asio::signal_set> ptr;
	return make_shared(ptr, io);
}
