#include "stdafx.h"
#include "log_tools.hpp"

bool log_tools::log_init(boost::log::trivial::severity_level level, std::string name)
{
	if (!name.empty() && name.back() != '_')
		name += '_';

	boost::log::add_file_log(
		boost::log::keywords::file_name = (name + "auto_%N.log").c_str(),                                        /*< file name pattern >*/
		boost::log::keywords::rotation_size = 10 * 1024 * 1024,                                   /*< rotate files every 10 MiB... >*/
		boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0), /*< ...or at midnight >*/
		boost::log::keywords::format = "[%TimeStamp%]: %Message%"                                 /*< log record format >*/
	);

	/*
	boost::log::core::get()->set_formatter(
	expr::format("[%1%]<%2%>(%3%)(%4%): %5%")
	% expr::attr<unsigned int>("LineID")
	% expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
	% expr::attr<sign_severity_level>("Severity")
	% expr::attr<attrs::current_thread_id::value_type >("ThreadID")
	% expr::smessage
	);
	*/

	//BOOST_LOG_ATTRIBUTE_KEYWORD(log_timestamp, "TimeStamp", boost::posix_time::ptime);
	boost::log::core::get()->set_filter(boost::log::trivial::severity >= level);
	boost::log::core::get()->add_global_attribute("TimeStamp", boost::log::attributes::local_clock());
	// boost::log::core::get()->add_global_attribute("LineID", boost::log::attributes::counter< unsigned int >(1));




	/*
	BOOST_LOG_TRIVIAL(trace) << "A trace severity message";
	BOOST_LOG_SEV(lg, boost::log::trivial::debug) << "A debug severity message";
	BOOST_LOG_SEV(lg, boost::log::trivial::info) << "An informational severity message";
	BOOST_LOG_SEV(lg, boost::log::trivial::warning) << "A warning severity message";
	BOOST_LOG_SEV(lg, boost::log::trivial::error) << "An error severity message";
	BOOST_LOG_SEV(lg, boost::log::trivial::fatal) << "A fatal severity message";
	*/

	return true;
}

void log_tools::log_flush()
{
	boost::log::core::get()->flush();
}
