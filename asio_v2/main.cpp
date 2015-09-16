// asio_net.cpp : Defines the entry point for the console application.
//
// boost 1.59
// link: 
// cryptopp 562
// openssl 102d
// link: cryptlib.lib;libeay32.lib;ssleay32.lib;
// running: server.crt server.key for server
//          ca.crt for client

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

