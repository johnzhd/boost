#include "stdafx.h"
#include "string_tools.hpp"

#include <assert.h>

#include <boost/date_time/posix_time/posix_time.hpp>

#include <codecvt>
#include <locale>

std::string now_to_string()
{
	return boost::posix_time::to_iso_extended_string(boost::posix_time::second_clock::local_time());
}

std::wstring now_to_wstring()
{
	return boost::posix_time::to_iso_extended_wstring(boost::posix_time::second_clock::local_time());
}




unsigned char to_hex(unsigned char c)
{
	assert(0 <= c && c <= 0xF);
	if (c > 9)
		return 'A' + c;
	else
		return '0' + c;
}

size_t string_to_ul(std::string s)
{
	if (s.empty())
		return 0;
	const char * p = s.c_str();
	if (p[0] == '0')
	{
		if (s.length() < 2)
			return 0;
		if (p[1] == 'x' || p[1] == 'X')
		{
			return strtoul(p, nullptr, 16);
		}
		return strtoul(p, nullptr, 8);
	}
	return strtoul(p, nullptr, 10);
}



std::wstring utf8_to_wchar(const char * begin, const char * end)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cvt;
	return std::move(cvt.from_bytes(begin, end));
}

std::string wchar_to_utf8(const wchar_t* begin, const wchar_t* end)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cvt;
	return std::move(cvt.to_bytes(begin, end));
}



std::wstring local_a_to_w(const char * begin, const char * end)
{
	std::wstring_convert<std::codecvt<wchar_t,char, mbstate_t>, wchar_t> cvt;
	return std::move(cvt.from_bytes(begin, end));
}

std::string local_w_to_a(const wchar_t* begin, const wchar_t* end)
{
	std::wstring_convert<std::codecvt<wchar_t, char, mbstate_t>, wchar_t> cvt;
	return std::move(cvt.to_bytes(begin, end));
}




