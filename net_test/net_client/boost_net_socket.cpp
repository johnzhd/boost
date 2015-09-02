#include "stdafx.h"
#include "boost_net_socket.hpp"


#include "boost_net_base.hpp"

boost::asio::ip::tcp::endpoint dns(boost::asio::yield_context yc,
	boost::system::error_code& ec,
	boost::asio::ip::tcp::resolver& resolver,
	std::string address,
	std::string port)
{
	boost::asio::ip::tcp::resolver::query query(address, port);
	auto it = resolver.async_resolve(query, yc[ec]);
	if (ec)
		return boost::asio::ip::tcp::endpoint();
	return *it;
}



http_base::http_base(boost::asio::io_service& io)
	:s_opt(io)
{

}

http_base::~http_base(void)
{

}

bool http_base::connect(boost::asio::yield_context& yc, boost::system::error_code& ec, boost::asio::ip::tcp::endpoint ep)
{
	s_opt.async_connect(ep, yc[ec]);
	//boost::asio::async_connect(s_opt, it, yc[ec]);
	if (ec)
	{
		return false;
	}
	return true;
}

void http_base::cancel(void)
{
	boost::system::error_code ec;
	s_opt.cancel(ec);
	if (ec)
	{
		auto str = ec.message();
	}
}

bool http_base::get_extern_info(std::vector<unsigned char>& ret)
{
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////
https_base::https_base(boost::asio::io_service& io)
	:s_opt(io, boost::asio::ssl::context(boost::asio::ssl::context::sslv23_client))
{

}

https_base::~https_base(void)
{

}

bool https_base::connect(boost::asio::yield_context& yc, boost::system::error_code& ec, boost::asio::ip::tcp::endpoint ep)
{
	s_opt.set_verify_mode(boost::asio::ssl::verify_peer, ec);
	if (ec)
	{
		return false;
	}
	s_opt.set_verify_callback(boost::bind(https_base::verify_certificate, this, _1, _2), ec);
	if (ec)
	{
		return false;
	}

	s_opt.lowest_layer().async_connect(ep, yc[ec]);
	//boost::asio::async_connect(s_opt.lowest_layer(), it, yc[ec]);
	if (ec)
	{
		return false;
	}

	s_opt.async_handshake(boost::asio::ssl::stream_base::client, yc[ec]);
	if (ec)
	{
		return false;
	}
	return true;
}

void https_base::cancel(void)
{
	s_opt.lowest_layer().cancel();
}

bool https_base::get_extern_info(std::vector<unsigned char>& ret)
{
	if (extern_info.empty())
		return false;
	ret.clear();
	extern_info.swap(ret);
	return true;
}






