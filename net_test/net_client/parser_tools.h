#pragma once

#include <string>
#include <vector>
#include <sstream>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>



class json_parser
{
public:
	template<typename Ch>
	json_parser(std::vector<Ch>& v)
	{
		typedef typename std::vector<Ch>::iterator It;

		// Load data into vector
		// Prepare grammar
		boost::property_tree::json_parser::json_grammar<boost::property_tree::ptree> g;

		// Parse
		try
		{
			boost::spirit::classic::parse_info<It> pi = boost::spirit::classic::parse(v.begin(), v.end(), g,
				boost::spirit::classic::::space_p | boost::spirit::classic::comment_p("//") | boost::spirit::classic::comment_p("/*", "*/"));
			if (pi.hit && pi.full)
			{
				// Swap grammar context root and pt
				core.swap(g.c.root);
				return;
			}
			
			str_err = "syntax error";
			
		}
		catch (boost::spirit::classic::parser_error<std::string, It> &e)
		{
			std::stringstream ss;
			ss << "Err: " << std::string(e.descriptor)
				<< ", line: "
				<< boost::property_tree::json_parser::count_lines<It, Ch>(v.begin(), e.where);
			str_err = ss.str();
		}

		
	}

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
	template<typename Ch>
	xml_parser(Ch* ptr, int flags = 0)
		:core()
	{
		try
		{
			boost::property_tree::detail::rapidxml::xml_document<Ch> doc;
			const int f_tws = boost::property_tree::detail::rapidxml::parse_normalize_whitespace
				| boost::property_tree::detail::rapidxml::parse_trim_whitespace;
			const int f_c = boost::property_tree::detail::rapidxml::parse_comment_nodes;
			// Some compilers don't like the bitwise or in the template arg.
			const int f_tws_c = boost::property_tree::detail::rapidxml::parse_normalize_whitespace
				| boost::property_tree::detail::rapidxml::parse_trim_whitespace
				| boost::property_tree::detail::rapidxml::parse_comment_nodes;
			if (flags & boost::property_tree::xml_parser::no_comments) {
				if (flags & boost::property_tree::xml_parser::trim_whitespace)
					doc.BOOST_NESTED_TEMPLATE parse<f_tws>(ptr);
				else
					doc.BOOST_NESTED_TEMPLATE parse<0>(ptr);
			}
			else {
				if (flags & boost::property_tree::xml_parser::trim_whitespace)
					doc.BOOST_NESTED_TEMPLATE parse<f_tws_c>(ptr);
				else
					doc.BOOST_NESTED_TEMPLATE parse<f_c>(ptr);
			}

			boost::property_tree::ptree local;
			for (boost::property_tree::detail::rapidxml::xml_node<Ch> *child = doc.first_node();
			child; child = child->next_sibling())
				boost::property_tree::xml_parser::read_xml_node(child, local, flags);

			// Swap local and result ptrees
			core.swap(local);
		}
		catch (const boost::property_tree::xml_parser_error&e)
		{
			str_err = e.message();
		}
	}
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

