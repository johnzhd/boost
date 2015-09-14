#pragma once






#include <vector>
#include <list>
#include <iostream>
#include <fstream>

#include "task_work.h"


template<typename T_work_base = task_work_base>
class echo_server
{
protected:
	protocal_type s_type;
	T_work_base base_core;
	boost::asio::ip::tcp::endpoint bind_endpoint;
	boost::function<void(boost::asio::yield_context yc)> work_loop_function;
public:
	echo_server(boost::shared_ptr<net_thread> net)
		:base_core(net), work_loop_function(boost::bind(&echo_server<T_work_base>::work_loop_enter, this, _1))
	{

	}
	~echo_server() {}

public:
	bool start_work()
	{
		bind_endpoint = to_endpoint(0, 18080);
		s_type = HTTP;
		base_core.spawn_start(work_loop_function);
		return true;
	}
public:
	void work_loop_enter(boost::asio::yield_context yc)
	{
		boost::system::error_code ec;
		boost::shared_ptr<b_net_acceptor_api> acceptor_opt;
		acceptor_opt = base_core.make_acceptor();

		while (!ec)
		{
			auto ptr = acceptor_opt->make_server_socket(s_type);
			acceptor_opt->accept(yc[ec], ptr);
			if (ec)
			{
				auto str = ec.message();
			}

			// I do not learn boost well.
			// This kind of no holder lambda or temp function object will cause memory exception inside spawn when calling back.
			// So exploit it...
			boost::asio::spawn(base_core.get_io(), boost::bind(&echo_server<T_work_base>::echo_loop_enter, this, _1, ptr));
		}
	}

	void echo_loop_enter(boost::asio::yield_context yc, boost::shared_ptr<b_net_socket_api> s_opt)
	{
		boost::system::error_code ec;
		std::vector<char> buff;
		size_t n;
		s_opt->handshake(yc[ec], boost::asio::ssl::stream_base::server);
		if(ec)
		{
			auto str = ec.message();
			return;
		}

		buff.resize(2048);
		while (!ec)
		{
			if (0 == (n = s_opt->recv(yc, ec, buff)) || ec)
			{
				ec.message();
				return;
			}

			s_opt->send_at_least(yc, ec, n, buff);
		}
	}
};