#include "stdafx.h"
#include "parser_tools.h"

#include <sstream>

json_parser::~json_parser() {}

std::string json_parser::make()
{
	std::stringstream ss;
	try
	{
		boost::property_tree::write_json(ss, core);
		return ss.str();
	}
	catch (const boost::property_tree::json_parser_error&e)
	{
		str_err = e.message();
	}
	return std::string();
}

std::string json_parser::get_last_err()
{
	return str_err;
}

xml_parser::~xml_parser() {}

std::string xml_parser::make()
{
	std::stringstream ss;
	try
	{
		boost::property_tree::write_xml(ss, core);
		return ss.str();
	}
	catch (const boost::property_tree::xml_parser_error&e)
	{
		str_err = e.message();
	}
	return std::string();
}

std::string xml_parser::get_last_err()
{
	return str_err;
}
