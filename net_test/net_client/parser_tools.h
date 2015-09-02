#pragma once

#include <string>
#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>

class json_parser
{
public:
	json_parser(std::string text);
	~json_parser();

	std::string make();
public:
	std::string get_last_err();

	template<typename T>
	T get_value(std::string path, T default_value)
	{
		return core.get(path, default_value);
	}

	template<typename T>
	T set_value(std::string path, T value)
	{
		core.add(path, value);
	}

public:
	boost::property_tree::ptree core;
	std::string str_err;
};


class xml_parser
{
public:
	xml_parser(std::string text);
	~xml_parser();

	std::string make();
public:
	std::string get_last_err();

	template<typename T>
	T get_value(std::string path, T default_value)
	{
		return core.get(path, default_value);
	}

	template<typename T>
	T set_value(std::string path, T value)
	{
		core.add(path, value);
	}

public:
	boost::property_tree::ptree core;
	std::string str_err;
};

