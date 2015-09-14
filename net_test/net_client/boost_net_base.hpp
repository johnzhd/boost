#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <boost/function.hpp>

#include <boost/format.hpp>

#include "boost_log.h"

#include "net_tools.h"


template<typename T, typename...Args>
inline boost::shared_ptr<T> make_shared(boost::shared_ptr<T>& ptr, Args&... args)
{
	return (ptr = boost::make_shared<T>(args...));
}

template<typename T>
inline std::string to_string(const T& t)
{
	boost::format f("%1%");
	f % t;
	return f.str();
}


inline boost::format& format_core(boost::format& f)
{
	return f;
}

template<typename T>
boost::format& format_core(boost::format& f, const T& t1)
{
	f % t1;
	return f;
}

template<typename T, typename...Args>
boost::format& format_core(boost::format& f, T& t1, Args&... args)
{
	f % t1;
	return format_core(f, args...);
}


template<typename T, typename...Args>
inline std::string format_string(T& format, const Args&... args)
{
	boost::format f(format);
	return format_core(f, args...).str();
}

template<typename F_D>
class my_RAII
{
public:
	my_RAII(F_D fd)
		:d(fd)
	{
	}
	~my_RAII()
	{
		d();
	}
protected:
	F_D d;
};




