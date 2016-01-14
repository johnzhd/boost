#include "stdafx.h"
#include "format_convert.hpp"

std::string cast_json(const boost::property_tree::ptree & p)
{
	std::stringstream ss;
	boost::property_tree::json_parser::write_json(ss, p, false);
	return std::move(ss.str());
}

std::wstring cast_json(const boost::property_tree::wptree & p)
{
	std::wstringstream ss;
	boost::property_tree::json_parser::write_json(ss, p, false);
	return std::move(ss.str());
}

boost::property_tree::ptree cast_json(const std::string & src)
{
	std::stringstream ss(src);
	boost::property_tree::ptree p;
	try
	{
		boost::property_tree::json_parser::read_json(ss, p);
	}
	catch (const boost::property_tree::json_parser_error)
	{
		p.clear();
	}
	return std::move(p);
}

boost::property_tree::wptree cast_json(const std::wstring & src)
{
	std::wstringstream ss(src);
	boost::property_tree::wptree p;
	try
	{
		boost::property_tree::json_parser::read_json(ss, p);
	}
	catch (const boost::property_tree::json_parser_error)
	{
		p.clear();
	}
	return std::move(p);
}

std::string cast_ip(unsigned int ip)
{
	boost::system::error_code ec;
	auto str = boost::asio::ip::address_v4(ip).to_string(ec);
	if (ec)
		return std::string();
	return std::move(str);
}

unsigned int cast_ip(std::string ip)
{

	boost::system::error_code ec;
	auto v = boost::asio::ip::address_v4::from_string(ip, ec);
	if (ec)
		return 0;
	return v.to_ulong();
}

boost::asio::ip::tcp::endpoint cast_endpoint(unsigned long address, unsigned short port)
{
	return std::move(boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4(address), port));
}

boost::asio::ip::tcp::endpoint cast_endpoint(std::string address, std::string port)
{
	boost::system::error_code ec;
	auto d = boost::asio::ip::address_v4::from_string(address, ec);
	if (ec)
		return std::move(boost::asio::ip::tcp::endpoint());
	return std::move(boost::asio::ip::tcp::endpoint(d, atoi(port.c_str())));
}

boost::asio::ip::tcp::endpoint cast_endpoint(std::string address, unsigned short port)
{
	boost::system::error_code ec;
	auto v = boost::asio::ip::address_v4::from_string(address, ec);
	if (ec)
		return std::move(boost::asio::ip::tcp::endpoint());
	return boost::asio::ip::tcp::endpoint(v, port);
}

boost::asio::ip::tcp::endpoint cast_endpoint(unsigned long address, std::string port)
{
	return std::move(boost::asio::ip::tcp::endpoint(boost::asio::ip::address_v4(address), atoi(port.c_str())));
}
