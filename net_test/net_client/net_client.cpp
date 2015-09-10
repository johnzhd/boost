// net_client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include "net_api.hpp"
#include "get_url_work.hpp"

int main()
{
	boost::shared_ptr<net_thread> thread_opt;
	boost::shared_ptr<get_url_work<>> work_opt;

	make_shared(thread_opt);

	make_shared(work_opt, thread_opt);

#if 1
	if (!work_opt->start_work())
		return 1;
#else
		std::function<void(boost::asio::yield_context yc)> f;
		{

			
			{
				f = std::bind(&get_url_work<>::work_loop_enter, work_opt, std::placeholders::_1);
			}
			std::function<void(boost::asio::yield_context yc)>& f2 = f;
			boost::asio::spawn(*thread_opt->get_io(), f2);
		}
#endif

	


	if (!thread_opt->run())
		return 1;


	boost::system::error_code ec;
	thread_opt->signal_handle(ec, SIGTERM);



    return 0;
}

