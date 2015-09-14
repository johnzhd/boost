#include "stdafx.h"
#include "boost_net_socket.hpp"


#include "boost_net_base.hpp"

boost::asio::ip::tcp::endpoint dns(boost::asio::yield_context yc,
	boost::system::error_code& ec,
	boost::asio::ip::tcp::resolver& resolver,
	std::string address,
	std::string port)
{
	auto it = dns(yc[ec], resolver, address, port);
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

boost::asio::ip::tcp::socket& http_base::get_socket()
{
	return s_opt;
}

bool http_base::connect(boost::asio::yield_context yc, boost::system::error_code& ec, boost::asio::ip::tcp::endpoint ep)
{
	connect(yc[ec],ep);
	//boost::asio::async_connect(s_opt, it, yc[ec]);
	return !ec;
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

https_base::https_base(boost::asio::io_service& io, boost::shared_ptr<boost::asio::ssl::context> ctx)
	: s_opt(io, *ctx)
{

}

https_base::~https_base(void)
{

}

bool https_base::connect(boost::asio::yield_context yc, boost::system::error_code& ec, boost::asio::ip::tcp::endpoint ep)
{
	connect(yc[ec], ep);
	if (ec)
		return false;
	
	handshake(yc[ec]);
	if (ec)
		return false;
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

http_server_base::http_server_base(boost::asio::io_service & io)
{
	make_shared(resolver_opt, io);
	make_shared(accept_opt, io);
}

http_server_base::~http_server_base()
{
}

boost::asio::io_service & http_server_base::get_io()
{
	return accept_opt->get_io_service();
}

void http_server_base::bind_listen(boost::system::error_code & ec, boost::asio::ip::tcp::endpoint ep)
{
	accept_opt->open(ep.protocol(), ec);
	if (ec)
		return;
	accept_opt->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), ec);
	if (ec)
		return;
	accept_opt->bind(ep, ec);
	if (ec)
		return;
	accept_opt->listen(boost::asio::socket_base::max_connections, ec);
}

std::string http_server_base::get_password(std::size_t max_length, boost::asio::ssl::context_base::password_purpose purpose)
{
	static const std::string both_key("test"); // same as key.pem

	const std::string& key(purpose == boost::asio::ssl::context_base::for_reading ? both_key: both_key);

	if (key.length() < max_length)
		return key;
	else
		return key.substr(0, max_length);
}

boost::shared_ptr<boost::asio::ssl::context> http_server_base::new_context(boost::system::error_code & ec)
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

	ptr->set_password_callback(&http_server_base::get_password, ec);
	if (ec)
		return nullptr;

	ptr->use_certificate_chain_file("work.crt", ec);
	if (ec)
		return nullptr;

	ptr->use_private_key_file("work.key", boost::asio::ssl::context::pem, ec);
	if (ec)
		return nullptr;

	ptr->use_tmp_dh_file("dh512.pem", ec);
	if (ec)
		return nullptr;

	return ptr;
}
