#pragma once

#include <string>

#include <boost/format.hpp>

#include <boost/algorithm/string.hpp>


template<typename T>
std::string to_string(const T& t)
{
	boost::format f("%1%");
	f % t;
	return f.str();
}

template<typename T>
std::wstring to_wstring(const T& t)
{
	boost::wformat f("%1%");
	f % t;
	return f.str();
}


std::string now_to_string();
std::wstring now_to_wstring();

unsigned char to_hex(unsigned char c);


size_t string_to_ul(std::string s);



std::wstring utf8_to_wchar(const char* begin, const char* end);
std::string wchar_to_utf8(const wchar_t* begin, const wchar_t* end);

std::wstring local_a_to_w(const char* begin, const char* end);
std::string local_w_to_a(const wchar_t* begin, const wchar_t* end);


template<typename F>
inline F& format_core(F& f)
{
	return f;
}

template<typename F, typename T>
inline F& format_core(F& f, const T& t1)
{
	f % t1;
	return f;
}

template<typename F, typename T, typename...Args>
inline F& format_core(F& f, T& t1, Args&... args)
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


template<typename T, typename...Args>
inline std::wstring format_wstring(T& format, const Args&... args)
{
	boost::wformat f(format);
	return format_core(f, args...).str();
}

template<typename S = std::string>
std::vector<S> split_to_v(const S& src, const S& mark, bool b_union = true)
{
	size_t last = 0;
	size_t npos = 0;

	std::vector<S> ret;

	while (last < src.length())
	{
		npos = b_union ? src.find(mark, last) : src.find_first_of(mark, last);
		if (npos == S::npos)
		{
			npos = src.length();
		}
		ret.push_back(std::move(src.substr(last, npos - last)));
		last = npos + (b_union ? mark.length() : 1);
	}

	return std::move(ret);
}



