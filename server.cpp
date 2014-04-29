#include "stdafx.h"
#include "server.h"

#include <assert.h>

namespace de_asio_server{

server::server()
{
}

server::~server()
{
}

void server::stop_server()
{
	boost::system::error_code ec;
	if ( m_Thread )
	{
		m_Thread = nullptr;
	}
	if ( m_Listen )
	{
		auto ptr = m_Listen;
		m_Listen = nullptr;
		ptr->cancel(ec);
	}

	if ( m_IoService )
	{
		m_IoService->stop();
		m_IoService = nullptr;
	}

}


server::Result_Type server::run_server( std::string strBindIP, std::string strBindPort,
							   work_func_type work_func, accept_func_type accept_func, boost::system::error_code& ec )
{
	if ( m_Thread != nullptr )
	{
		return err_exist;
	}

	if ( m_IoService == nullptr )
	{
		m_IoService = boost::make_shared<boost::asio::io_service>();
	}

	m_BindIP = strBindIP;
	m_nBindPort = strBindPort;

	if ( work_func )
	{
		m_work_func = work_func;
	}
	else
	{
		m_work_func = nullptr;
	}

	if (accept_func)
	{
		boost::asio::spawn(*m_IoService,accept_func);
	}
	else
	{
		boost::asio::spawn(*m_IoService,boost::bind(&server::accept_core,this,_1));
	}

	try
	{
		m_IoService->run();
		if (0)
		{
		m_Thread = boost::make_shared<boost::thread>(boost::bind(&boost::asio::io_service::run, m_IoService));
		if ( m_Thread )
			m_Thread->join();
		}
	}
	catch(boost::system::error_code e)
	{
		ec = e;
		return err_ec;
	}
	catch(...)
	{
		return err_unkown;
	}

	return success;
}

void server::accept_core(boost::asio::yield_context yield)
{
	assert(m_IoService);
	m_Listen = boost::make_shared<boost::asio::ip::tcp::acceptor>(*m_IoService);
	{
		boost::asio::ip::tcp::resolver resolver(*m_IoService);
		boost::asio::ip::tcp::resolver::query query(m_BindIP, m_nBindPort);
		boost::asio::ip::tcp::endpoint local_endpoint_ = *resolver.resolve(query);
		m_Listen->open(local_endpoint_.protocol());
		m_Listen->set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
		m_Listen->bind(local_endpoint_);
		m_Listen->listen(); // throw ec for listening err.
		// listen is system action.
	}

	boost::system::error_code ec;
	for (;m_Listen && m_IoService;)
	{
		boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ =boost::make_shared<boost::asio::ip::tcp::socket>(*m_IoService);
		m_Listen->async_accept(*socket_, yield[ec]);
		if ( ec )
		{
			// bad call
			if ( socket_ )
			{
				socket_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec); // Don't throw single action ec.
				// don't care ec
				socket_->close(ec);
			}
			continue;
		}

		if ( m_work_func == nullptr )
		{
			if ( socket_ )
			{
				socket_->shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec); // Don't throw single action ec.
				// don't care ec
				socket_->close(ec);
			}
		}

		boost::asio::spawn(*m_IoService,boost::bind(&server::work_shell,this,_1,socket_));
	};
}






void server::work_shell(boost::asio::yield_context yield,
		boost::shared_ptr<boost::asio::ip::tcp::socket> socket_)
{
	// ...
	if ( m_work_func )
	{
		m_work_func(yield, socket_ );
	}
}
















};

