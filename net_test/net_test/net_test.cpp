// net_test.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"


#include "net_http.h"

#include <boost/asio/spawn.hpp>

#include <string>
#include <vector>
int _tmain(int argc, TCHAR* argv[])
{

	net_http nh;

	auto io_ptr = boost::make_shared<boost::asio::io_service>();

	boost::asio::spawn(*io_ptr, boost::bind(&net_http::test,&nh,_1,io_ptr,std::string("http://www.163.com/")));

	try
	{
	io_ptr->run();
	}
	catch(...)
	{
	}

	io_ptr = nullptr;

	return 0;
}