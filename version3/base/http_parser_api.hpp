#pragma once

#include "http_parser.h"

#include <string>
#include <vector>
#include <list>
#include <map>


class http_parser_api
{
public:
	http_parser_api();
	~http_parser_api();
protected:
	void init(http_parser_type type = HTTP_BOTH);

	bool parser_execute(const char *data, size_t len);

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
	size_t head_code;
	std::string head_state;
	std::map<std::string, std::string> head_content;
	size_t head_npos;
	std::vector<char> head_list;
	std::list<std::vector<char>> body_list;


	void clear();
public:
	void push(const char* buff, size_t length);
	bool is_head_done();
	bool is_done();
	bool is_err();
	std::string get_err();

	std::string get_head(std::string key);
	bool search_head(std::string key, std::string match);
};


bool url_parser_simple(std::string src, std::string& ptl, std::string& domain, std::string& path);


bool url_parser(std::string url, std::string& schema, std::string& domain, std::string& port, std::string& path, std::string& param);


