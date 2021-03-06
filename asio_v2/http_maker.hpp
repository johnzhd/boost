#pragma once

#include <string>

#include "http_parser_api.h"

inline std::string http_request_get(std::string domain)
{
	return "GET / HTTP/1.1\r\nAccept: *.*\r\nHost: " + domain + ":80\r\n\r\n";
}

inline std::string http_request_url(std::string schema, std::string host, std::string port, std::string path, std::string param, bool b_post)
{
	if (b_post)
	{
		return format_string("POST %1% HTTP/1.1\r\n"
			"Content-Type: application/x-www-form-urlencoded\r\n"
			"Content-Length: %4%\r\n"
			"Connection: Keep-Alive\r\n"
			"DNT: 1\r\n"
			"Host: %3%\r\n"
			"User-Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)\r\n"
			"\r\n"
			"%2%", path, param, host, param.length());
	}
	else
	{
		return format_string("GET %1%%4%%2% HTTP/1.1\r\n"
			"Accept: text/html, application/xhtml+xml, image/jxr, */*\r\n"
			"Accept-Encoding: gzip, deflate\r\n"
			"Accept-Language: zh-CN\r\n"
			"Cache-Control: no-cache\r\n"
			"Connection: Keep-Alive\r\n"
			"DNT: 1\r\n"
			"Host: %3%\r\n"
			"User-Agent: Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; Trident/5.0)\r\n"
			"\r\n", path, param, host, (param.empty()?"":"?"));
	}
	return std::string();
}



inline std::string http_request_url(std::string url, bool b_post)
{
	std::string schema, host, port, path, param;
	if (!url_parser(url, schema, host, port, path, param))
		return std::string();
	return http_request_url(schema, host, port, path, param, b_post);
}




