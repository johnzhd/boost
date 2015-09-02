#include "stdafx.h"
#include "http_parser_api.h"


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
	return b_head_finished || b_finished;
}

