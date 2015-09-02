#pragma once

#include "http_parser.h"

#include <string>
#include <map>
#include <vector>
#include <list>
#include <algorithm>

typedef const struct http_parser_settings c_st_settings_type;

template<typename T>
class http_parser_tools
{
public:
	static int on_message_begin(http_parser* p) {
		return ((T*)p->data)->on_message_begin(p);
	}
	static int on_headers_complete(http_parser* p) {
		return ((T*)p->data)->on_headers_complete(p);
	}
	static int on_message_complete(http_parser* p) {
		return ((T*)p->data)->on_message_complete(p);
	}
	static int on_chunk_header(http_parser* p) {
		return ((T*)p->data)->on_chunk_header(p);
	}
	static int on_chunk_complete(http_parser* p) {
		return ((T*)p->data)->on_chunk_complete(p);
	}
	static int on_url(http_parser* p, const char *at, size_t length) {
		return ((T*)p->data)->on_url(p,at,length);
	}
	static int on_status(http_parser* p, const char *at, size_t length) {
		return ((T*)p->data)->on_status(p, at, length);
	}
	static int on_header_field(http_parser* p, const char *at, size_t length) {
		return ((T*)p->data)->on_header_field(p, at, length);
	}
	static int on_header_value(http_parser* p, const char *at, size_t length) {
		return ((T*)p->data)->on_header_value(p, at, length);
	}
	static int on_body(http_parser* p, const char *at, size_t length) {
		return ((T*)p->data)->on_body(p, at, length);
	}

	static c_st_settings_type settings;
public:
	static c_st_settings_type * get_settings()
	{
		//if (settings)
		//{
		//	settings = new struct http_parser_settings();
		//	settings->on_message_begin = &http_parser_tools<T>::on_message_begin;
		//	settings->on_headers_complete = &http_parser_tools<T>::on_headers_complete;
		//	settings->on_message_complete = &http_parser_tools<T>::on_message_complete;
		//	settings->on_url = &http_parser_tools<T>::on_url;
		//	settings->on_status = &http_parser_tools<T>::on_status;
		//	settings->on_body = &http_parser_tools<T>::on_body;
		//	settings->on_header_field = &http_parser_tools<T>::on_header_field;
		//	settings->on_header_value = &http_parser_tools<T>::on_header_value;
		//}
		return &settings;
	}
};



template<typename T>
c_st_settings_type http_parser_tools<T>::settings = {
	&http_parser_tools<T>::on_message_begin,
	&http_parser_tools<T>::on_url,
	&http_parser_tools<T>::on_status,
	&http_parser_tools<T>::on_header_field,
	&http_parser_tools<T>::on_header_value,
	&http_parser_tools<T>::on_headers_complete,
	&http_parser_tools<T>::on_body,
	&http_parser_tools<T>::on_message_complete,

	&http_parser_tools<T>::on_chunk_header,
	&http_parser_tools<T>::on_chunk_complete };



class http_parser_api
{
public:
	http_parser_api();
	~http_parser_api();
protected:
	void init(http_parser_type type = HTTP_BOTH);

	bool parser_execute(const char *data, size_t len);

	std::string get_err();
public:
	int on_message_begin(http_parser* p);
	int on_headers_complete(http_parser* p);
	int on_message_complete(http_parser* p);
	int on_chunk_header(http_parser* p);
	int on_chunk_complete(http_parser* p);
	int on_url(http_parser* p, const char *at, size_t length);
	int on_status(http_parser* p, const char *at, size_t length);
	int on_header_field(http_parser* p, const char *at, size_t length);
	int on_header_value(http_parser* p, const char *at, size_t length);
	int on_body(http_parser* p, const char *at, size_t length);
protected:
	struct http_parser parser;

	std::string head_key;
	std::string head_value;


	bool b_head_finished;
	bool b_finished;
	
public:
	std::string head_state;
	std::map<std::string, std::string> head_content;
	std::list<std::vector<char>> body_list;


	void clear();
public:
	void push(const char* buff, size_t length);
	bool is_done();
	bool is_err();
};


inline bool url_parser_simple(std::string src, std::string& ptl, std::string& domain, std::string& path)
{
	size_t npos, offset;
	npos = src.find("://");
	if (npos == std::string::npos)
	{
		ptl = "http";
		npos = 0;
	}
	else
	{
		ptl = src.substr(0, npos);
		std::transform(ptl.begin(), ptl.end(), ptl.begin(), tolower);
		npos += 3;
	}

	offset = src.find("/", npos);
	if (offset == std::string::npos)
	{
		domain = src.substr(npos);
		path = "/";
	}
	else
	{
		domain = src.substr(npos, offset - npos);
		path = src.substr(offset);
	}
	return true;
}



