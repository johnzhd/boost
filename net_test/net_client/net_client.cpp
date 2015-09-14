// net_client.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "net_api.hpp"
#include "get_url_work.hpp"
#include "echo_server.hpp"

int main()
{
	boost::shared_ptr<net_thread> thread_opt;
	boost::shared_ptr<echo_server<>> work_opt;

	make_shared(thread_opt);

	make_shared(work_opt, thread_opt);

	if (!work_opt->start_work())
		return 1;


	


	if (!thread_opt->run())
		return 1;


	boost::system::error_code ec;
	thread_opt->signal_handle(ec, SIGTERM);



    return 0;
}

