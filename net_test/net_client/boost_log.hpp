#pragma once


#include <boost/date_time/posix_time/posix_time.hpp>


#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>

template<boost::log::trivial::severity_level T>
void log_init()
{
	boost::log::add_file_log
		(
			boost::log::keywords::file_name = "sample_%N.log",                                        /*< file name pattern >*/
			boost::log::keywords::rotation_size = 10 * 1024 * 1024,                                   /*< rotate files every 10 MiB... >*/
			boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0), /*< ...or at midnight >*/
			boost::log::keywords::format = "[%TimeStamp%]: %Message%"                                 /*< log record format >*/
			);

	//BOOST_LOG_ATTRIBUTE_KEYWORD(log_timestamp, "TimeStamp", boost::posix_time::ptime);
	boost::log::core::get()->set_filter(boost::log::trivial::severity >= T);
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
}

#define boost_log(l, ...) do{ BOOST_LOG_TRIVIAL(l) << format_string(__VA_ARGS__); }while(false)


#include <boost/timer/timer.hpp>
#include <string>

#define DEBUG_TIMER() boost::timer::auto_cpu_timer t(BOOST_LOG_TRIVIAL(trace) , std::string(__FUNCTION__) + ": %ws wall, %us user + %ss system = %ts CPU (%p%)\n");



