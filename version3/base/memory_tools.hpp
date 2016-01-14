#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>

#include <memory>

template<typename T, typename...Args>
inline boost::shared_ptr<T> make_shared(boost::shared_ptr<T>& ptr, Args&... args)
{
	return (ptr = boost::make_shared<T>(args...));
}

template<typename T, typename...Args>
inline std::shared_ptr<T> make_shared(std::shared_ptr<T>& ptr, Args&... args)
{
	return (ptr = std::make_shared<T>(args...));
}

