
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



void test_core(boost::shared_ptr<boost::asio::io_service> server_, boost::asio::yield_context yield)
{
	unsigned long t_id = GetCurrentThreadId();

	printf("thread_id: %u", t_id);
	// socket 
	boost::asio::ip::tcp::socket socket_(*server_);
	std::string address,port,data;
	std::vector<unsigned char> recv_data;
	boost::system::error_code ec;
	

	address="www.baidu.com";
	port="80";
	boost::asio::ip::tcp::resolver resolver(*server_);
	boost::asio::ip::tcp::resolver::query query(address, port);

	boost::asio::ip::tcp::resolver::iterator resolver_it;

	resolver_it = resolver.async_resolve(query,yield[ec]);
	if ( ec )
	{
		printf( "%s (%s): %d %s\n", __FILE__,__LINE__,ec.value(), ec.message() );
		return ;
	};
	

	// connect 
	socket_.async_connect(*resolver_it, yield[ec]);

	if ( ec )
	{
		printf( "%s (%s): %d %s\n", __FILE__,__LINE__,ec.value(), ec.message() );
		return ;
	}

	// send 
	data = "GET / HTTP/1.1\r\n"\
		"Host: www.baidu.com:80\r\n"\
		"Accept: *.*\r\n"\
		"\r\n";

	std::size_t length = socket_.async_write_some( boost::asio::buffer(data.c_str(), data.length()),yield[ec]);

	if ( ec )
	{
		printf( "%s (%s): %d %s\n", __FILE__,__LINE__,ec.value(), ec.message() );
		return ;
	};


	// recv
	recv_data.resize( 2048,0);
	length = socket_.async_read_some(
           boost::asio::buffer(recv_data), yield[ec]);

	if ( ec )
	{
		printf( "%s (%s): %d %s\n", __FILE__,__LINE__,ec.value(), ec.message() );
		return ;
	};

	for ( auto it = recv_data.begin(); it != recv_data.end() && *it != '\n'; it++ )
	{
		printf("%c", *it);
	};
	printf("\n");
	
	socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
	// don't care ec
	socket_.close();

}


bool work_net_commond( const unsigned char * commond, size_t common_length,
					  boost::array<unsigned char, 1024>& response, size_t* response_length )
{
	return true;
};


void start_accept_core(boost::asio::yield_context yield, boost::shared_ptr<boost::asio::io_service> server_, std::string address, std::string port)
{
	boost::asio::ip::tcp::acceptor ptr_(*server_);
	boost::asio::ip::tcp::resolver resolver(*server_);
	boost::asio::ip::tcp::resolver::query query(address, port);
	boost::asio::ip::tcp::endpoint local_endpoint_ = *resolver.resolve(query);
	ptr_.open(local_endpoint_.protocol());
	ptr_.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	ptr_.bind(local_endpoint_);
	ptr_.listen();


	boost::array<unsigned char, 1024> buffer_temp;
	size_t length_temp;

	boost::system::error_code ec;
	for (;;)
	{
		boost::asio::ip::tcp::socket socket_(*server_);
		auto close_socket=[&](){
			socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
			// don't care ec
			socket_.close();
		};
		ptr_.async_accept(socket_, yield[ec]);
		if ( ec )
		{
			// bad call
			close_socket();
			continue;
		};

		length_temp = socket_.async_read_some( boost::asio::buffer(buffer_temp), yield[ec] );
		if ( ec )
		{
			close_socket();
			continue;
		};
		
		if ( length_temp==0 || false == work_net_commond(buffer_temp.c_array(), length_temp, buffer_temp, &length_temp) )
		{
			close_socket();
			continue;
		};

		socket_.async_write_some(boost::asio::buffer(buffer_temp,length_temp),yield[ec]);
		if ( ec )
		{
			// log
		};
		close_socket();
	};
};

int main(int argc, char* argv[])
{
	auto server_ = boost::make_shared<boost::asio::io_service>();
		//work_ptr work(new boost::asio::io_service::work(*io_service));

	//boost::thread t(test_func);
	boost::asio::spawn(*server_,boost::bind(start_accept_core,_1,server_,"0.0.0.0","18080"));
//	boost::asio::spawn(*server_,boost::bind(test_core,server_,_1));

	printf("Start\n");
	try
	{
		boost::shared_ptr<boost::thread> thread(new boost::thread(
			boost::bind(&boost::asio::io_service::run, server_)));

		thread->join();
	}
	catch(...)
	{
	};

	return 0;
};