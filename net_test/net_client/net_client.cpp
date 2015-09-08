// net_client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "net_api.hpp"
#include "task_work.h"

int main()
{
	boost::shared_ptr<net_thread> thread_opt;
	boost::shared_ptr<task_work> work_opt;

	make_shared(thread_opt);

	make_shared(work_opt, thread_opt);

	work_opt->task_list.push_back("http://www.jisilu.cn/data/cf/cf_list/?___t=1441521364542");

	thread_opt->spawn_start(boost::bind(&task_work::timer_loop, work_opt, _1));
	thread_opt->spawn_start(boost::bind(&task_work::work_loop, work_opt, _1));

	thread_opt->run();


	boost::system::error_code ec;
	thread_opt->signal_handle(ec, 1);



    return 0;
}

