#pragma once

#include <string>
#include <boost/asio.hpp>


inline std::string to_ip(unsigned int ip)
{
	try
	{
		return boost::asio::ip::address_v4(ip).to_string();
	}
	catch (const boost::system::error_code& e)
	{
		assert(!e);
	}
	return std::string();
}

inline unsigned int to_ip(std::string ip)
{
	try
	{
		return boost::asio::ip::address_v4::from_string(ip).to_ulong();
	}
	catch (const boost::system::error_code& e)
	{
		assert(!e);
	}
	return 0;
}

inline boost::asio::ip::tcp::endpoint to_endpoint(unsigned long address, unsigned short port)
{
	try
	{
		return boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4(address), port);
	}
	catch (const boost::system::error_code& e)
	{
		assert(!e);
	}
	return boost::asio::ip::tcp::endpoint();
}

inline boost::asio::ip::tcp::endpoint to_endpoint(std::string address, std::string port)
{
	try
	{
		return boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4::from_string(address), atoi(port.c_str()));
	}
	catch (const boost::system::error_code& e)
	{
		assert(!e);
	}
	return boost::asio::ip::tcp::endpoint();
}
