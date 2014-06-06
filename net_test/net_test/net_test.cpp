// net_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "http_work.h"
#include "net_base.hpp"

#include <string>
#include <vector>

#include <boost/bind.hpp>
#include <iostream>


net_work nw;

int on_url(http_parser* hp, const char *at, size_t length)
{
	std::cout << __FUNCTION__ << std::endl;
	return 0;
}
int on_status(http_parser* hp, const char *at, size_t length)
{
	std::cout << __FUNCTION__ << std::endl;
	return 0;
}
int on_header_field(http_parser* hp, const char *at, size_t length)
{
	std::cout << __FUNCTION__ << std::endl;
	return 0;
}
int on_header_value(http_parser* hp, const char *at, size_t length)
{
	std::cout << __FUNCTION__ << std::endl;
	return 0;
}
int on_body(http_parser* hp, const char *at, size_t length)
{
	std::cout << __FUNCTION__ << std::endl;
	return 0;
}
int on_message_begin(http_parser* hp)
{
	std::cout << __FUNCTION__ << std::endl;
	return 0;
}
int on_headers_complete(http_parser* hp)
{
	std::cout << __FUNCTION__ << std::endl;
	size_t n = http_should_keep_alive(hp);
	return 0;
}
int on_message_complete(http_parser* hp)
{
	std::cout << __FUNCTION__ << std::endl;
	size_t n = http_should_keep_alive(hp);
	return 0;
}


void func_send(boost::asio::yield_context yield)
{
	boost::system::error_code ec;
	auto s = nw.new_socket();
	nw.connect(yield,ec,s,"www.baidu.com", "80");
	if ( ec )
		return ;

	std::string request;
	request = "GET / HTTP/1.1\r\nAccept: *.*\r\nHost: www.baidu.com:80\r\n\r\n";
	size_t length = nw.send(yield,ec,s,request);

	std::vector<char> v_buff;
	v_buff.resize(2048);



	http_parser hp;
	http_parser_settings hps;
	http_parser_init(&hp,HTTP_RESPONSE);
	hps.on_message_begin = on_message_begin;
	hps.on_url = on_url;
	hps.on_status = on_status;
	hps.on_header_field = on_header_field;
	hps.on_header_value = on_header_value;
	hps.on_headers_complete = on_headers_complete;
	hps.on_body = on_body;
	hps.on_message_complete = on_message_complete;


	do
	{
	length = nw.recv(yield,ec,s,v_buff);
	if ( ec )
		break;
	size_t ret = http_parser_execute(&hp,&hps,&v_buff[0],length);
	if ( ret != length )
		break;
	}while ( 0 == http_body_is_final(&hp) );

	system("pause");
};

int _tmain(int argc, TCHAR* argv[])
{

	boost::asio::spawn(*nw.get_service(),func_send);

	//nw.pre(func_send);

	nw.run<bool>();
	return 0;
}