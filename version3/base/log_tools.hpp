#pragma once


#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

#include <string>

#include "string_tools.hpp"


namespace log_tools
{
	bool log_init(boost::log::trivial::severity_level level, std::string name = std::string());
	void log_flush();


	template<typename ... ARGS>
	void boost_log(boost::log::trivial::severity_level level, bool b_flush, ARGS ...args)
	{
		BOOST_LOG_SEV(boost::log::core::get(), level) << format_string(args...);
		if (b_flush)
			log_flush();
	}

	template<typename ... ARGS>
	void boost_log_trace(ARGS ...args)
	{
		boost_log(boost::log::trivial::trace, true, args...);
	}

	template<typename ... ARGS>
	void boost_log_debug(ARGS ...args)
	{
		boost_log(boost::log::trivial::debug, true, args...);
	}

	template<typename ... ARGS>
	void boost_log_info(ARGS ...args)
	{
		boost_log(boost::log::trivial::info, true, args...);
	}

	template<typename ... ARGS>
	void boost_log_warning(ARGS ...args)
	{
		boost_log(boost::log::trivial::warning, true, args...);
	}

	template<typename ... ARGS>
	void boost_log_error(ARGS ...args)
	{
		boost_log(boost::log::trivial::error, true, args...);
	}

	template<typename ... ARGS>
	void boost_log_fatal(ARGS ...args)
	{
		boost_log(boost::log::trivial::fatal, true, args...);
	}
};





















