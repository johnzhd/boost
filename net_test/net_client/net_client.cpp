// net_client.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include "net_api.hpp"

int main()
{
	boost::shared_ptr<net_thread> thread_opt;
	boost::shared_ptr<net_work> work_opt;

	make_shared(thread_opt);

	make_shared(work_opt, thread_opt);

	thread_opt->spawn_start(boost::bind(&net_work::demo_loop, work_opt, _1));

	thread_opt->run();


	boost::system::error_code ec;
	thread_opt->signal_handle(ec, 1);



    return 0;
}

