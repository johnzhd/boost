#include "stdafx.h"
#include "http_parser_api.hpp"

#include <algorithm>

#include <assert.h>

#include "string_tools.hpp"

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
		return ((T*)p->data)->on_url(p, at, length);
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



http_parser_api::http_parser_api()
{
	clear();
}


http_parser_api::~http_parser_api()
{
}

void http_parser_api::init(http_parser_type type)
{
	http_parser_init(&parser, type);
	parser.data = this;
}

bool http_parser_api::parser_execute(const char *data, size_t len)
{
	return 0 == http_parser_execute(&parser, http_parser_tools<http_parser_api>::get_settings(), data, len);
}

std::string http_parser_api::get_err()
{
	switch (HTTP_PARSER_ERRNO(&parser))
	{
#define work_gen(n, s) case HPE_##n: return s; break;
		HTTP_ERRNO_MAP(work_gen)
	default:
		break;
	}
	return "";
}

std::string http_parser_api::get_head(std::string key)
{
	auto it = head_content.find(key);
	if (it == head_content.end())
		return std::string();

	return it->second;
}

bool http_parser_api::search_head(std::string key, std::string match)
{
	auto it = head_content.find(key);
	if (it == head_content.end())
		return false;
	return it->second.find(match) != std::string::npos;
}

bool http_parser_api::is_err()
{
	return parser.http_errno != 0;
}

int http_parser_api::on_message_begin(http_parser* p) {
	return 0;
}
int http_parser_api::on_headers_complete(http_parser* p) {
	b_head_finished = true;
	head_npos = p->nread;
	return 0;
}
int http_parser_api::on_message_complete(http_parser* p) {
	b_finished = true;
	return 0;
}
int http_parser_api::on_chunk_header(http_parser* p) {
	p->content_length;
	return 0;
}
int http_parser_api::on_chunk_complete(http_parser* p) {
	return 0;
}
int http_parser_api::on_url(http_parser* p, const char *at, size_t length) {
	return 0;
}
int http_parser_api::on_status(http_parser* p, const char *at, size_t length) {
	head_code = p->status_code;
	head_state.assign(at, length);
	return 0;
}
int http_parser_api::on_header_field(http_parser* p, const char *at, size_t length) {
	head_key.assign(at, length);
	return 0;
}
int http_parser_api::on_header_value(http_parser* p, const char *at, size_t length) {
	head_value.assign(at, length);
	if (!head_key.empty() && !head_value.empty())
	{
		auto ret = head_content.insert(std::make_pair(head_key, head_value));
		if (ret.second == false)
		{
			ret.first->second += "; " + head_value;
		}
		head_key.clear();
		head_value.clear();
	}
	return 0;
}
int http_parser_api::on_body(http_parser* p, const char *at, size_t length) {
	std::vector<char> temp;
	temp.resize(length);
	std::copy(at, at + length, temp.begin());
	body_list.push_back(temp);
	return 0;
}

void http_parser_api::clear()
{
	init();
	head_key.clear();
	head_value.clear();
	head_content.clear();
	body_list.clear();

	b_head_finished = false;
	b_finished = false;
	head_npos = 0;
}

void http_parser_api::push(const char * buff, size_t length)
{
	parser_execute(buff, length);
	if (head_npos == 0)
	{
		head_list.assign(buff, buff+length);
	}
	else if (head_list.size() < head_npos)
	{
		size_t diff = head_npos - head_list.size();
		assert(diff <= length);
		head_list.insert(head_list.end(), buff, buff + (diff));
	}
}

bool http_parser_api::is_head_done()
{
	return b_head_finished;
}

bool http_parser_api::is_done()
{
	return b_finished;
}

bool url_parser_simple(std::string src, std::string & ptl, std::string & domain, std::string & path)
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

bool url_parser(std::string url, std::string & schema, std::string & domain, std::string & port, std::string & path, std::string & param)
{
	struct http_parser_url st_u;
	const char * p = url.c_str();
	memset(&st_u, 0, sizeof(st_u));

	if (0 != http_parser_parse_url(p, url.length(), false, &st_u))
		return false;


	if (st_u.field_set & (1 << UF_SCHEMA))
	{
		schema.assign(p + st_u.field_data[UF_SCHEMA].off, st_u.field_data[UF_SCHEMA].len);
	}
	else
	{
		schema = "http";
	}
	std::transform(schema.begin(), schema.end(), schema.begin(), tolower);

	if (st_u.field_set & (1 << UF_PORT))
	{
		port = to_string(st_u.port);
	}
	else
	{
		if (schema == "http")
		{
			port = "80";
		}
		else if (schema == "ftp")
		{
			port = "21";
		}
		else if (schema == "ssh")
		{
			port = "22";
		}
		else if (schema == "smtp")
		{
			port = "25";
		}
		else if (schema == "https")
		{
			port = "443";
		}
		else if (schema == "pop3")
		{
			port = "109";
		}
		else if (schema == "rpc")
		{
			port = "110";
		}
		else
		{
			port = "80";
		}
		
	}

	if (st_u.field_set & (1 << UF_HOST))
	{
		domain.assign(p + st_u.field_data[UF_HOST].off, st_u.field_data[UF_HOST].len);
	}

	if (st_u.field_set & (1 << UF_PATH))
	{
		path.assign(p + st_u.field_data[UF_PATH].off, st_u.field_data[UF_PATH].len);
	}
	else
	{
		path = "/";
	}

	if (st_u.field_set & (1 << UF_QUERY))
	{
		param.assign(p + st_u.field_data[UF_QUERY].off+1, st_u.field_data[UF_QUERY].len-1);
	}
	if (st_u.field_set & (1 << UF_FRAGMENT))
	{
		param.append(p + st_u.field_data[UF_FRAGMENT].off, st_u.field_data[UF_FRAGMENT].len);
	}
	return true;
}
