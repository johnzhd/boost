#pragma once

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>
#include <boost/thread/condition.hpp>

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/function.hpp>




#include <set>

namespace de_asio_server{

	
	//void work_demo( boost::asio::yield_context yield,
	//	boost::shared_ptr<boost::asio::io_service> service_,
	//	boost::asio::ip::tcp::socket socket_ )
	//{
	//	unsigned long t_id = GetCurrentThreadId();


	//	printf("thread_id: %u", t_id);
	//	
	//	// recv
	//	boost::system::error_code ec; 
	//	std::vector<char> recv_data;
	//	recv_data.resize( 2048,0);

	//	size_t length = socket_.async_read_some(
	//		boost::asio::buffer(recv_data), yield[ec]);
	//	if ( ec )
	//	{
	//		printf( "%s (%s): %d %s\n", __FILE__,__LINE__,ec.value(), ec.message() );
	//		return ;
	//	};

	//	recv_data.resize(length);
	//	for ( auto it = recv_data.begin(); it != recv_data.end() && *it != '\n'; it++ )
	//	{
	//		printf("%c", *it);
	//	};
	//	printf("\n");
	//}

class server
{
public:
	server();
	~server();
public:
	typedef std::function<void (boost::asio::yield_context yield,
		boost::shared_ptr<boost::asio::ip::tcp::socket> socket_)> work_func_type;

	typedef std::function<void (boost::asio::yield_context yield)> accept_func_type;

	typedef enum Result_Type
	{
		success = 0,
		err_exist,
		err_ec,
		err_unkown,
	} Result_Type;
public:
	inline boost::shared_ptr<boost::asio::io_service> get_io_service()
	{
		return m_IoService;
	}
public:
	Result_Type run_server( std::string strBindIP, std::string strBindPort,
		work_func_type work_func, accept_func_type accept_func, boost::system::error_code& ec );

	void stop_server();

protected:
	
	std::string m_BindIP;
	std::string m_nBindPort;
	
	work_func_type m_work_func;

	boost::shared_ptr<boost::asio::io_service> m_IoService;
	boost::shared_ptr<boost::thread> m_Thread;
	boost::shared_ptr<boost::asio::ip::tcp::acceptor> m_Listen;

protected:
	void accept_core(boost::asio::yield_context yield);
	void work_shell(boost::asio::yield_context yield,
		boost::shared_ptr<boost::asio::ip::tcp::socket> socket_);

private:



};

template<typename Contain = std::set<boost::asio::ip::tcp::socket> >
class socket_manager
{
public:
	socket_manager();
	~socket_manager();
public:
	void insert( boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ )
	{
		if ( socket_ )
		{
			boost::unique_lock<boost::mutex> lock(m_mutex);
			m_c.insert(socket_);
		}
	}
	void erase( boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ )
	{
		if ( socket_ )
		{
			boost::unique_lock<boost::mutex> lock(m_mutex);
			m_c.erase( socket_ );
		}
	}
	void cancel( boost::shared_ptr<boost::asio::ip::tcp::socket> socket_ )
	{
		if ( socket_ )
		{
			boost::system::error_code ec;
			socket_->cancel(ec);
			socket_->shutdown(boost::asio::socket_base::shutdown_type::shutdown_both,ec);
			socket_->close(ec);
		}
	}
	void cancel()
	{
		while ( false == m_c.empty() )
		{
			boost::unique_lock<boost::mutex> lock(m_mutex);
			auto it = m_c.begin();
			auto ptr = *it;
			
			m_c.erase(it);
			cancel(ptr);
		}
	}
protected:
	Contain m_c;
	boost::mutex m_mutex;
};


















};



