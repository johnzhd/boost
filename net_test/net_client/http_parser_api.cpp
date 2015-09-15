#include "stdafx.h"
#include "http_parser_api.h"

#include <algorithm>

#include "net_tools.hpp"

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
}

void http_parser_api::push(const char * buff, size_t length)
{
	parser_execute(buff, length);
}

bool http_parser_api::is_done()
{
	return b_finished;
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
		port = format_string("%1%", st_u.port);
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
