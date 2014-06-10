#include "stdafx.h"
#include "net_http.h"

#include <algorithm>



#include <gzip.h>
#pragma comment (lib,"cryptlib.lib")

void net_http::test(boost::asio::yield_context yield, boost::shared_ptr<boost::asio::io_service> io_ptr, std::string url)
{
	if ( false == url_info.parser_url(url) )
		return ;
	// url ==> package
	std::string str_get = net_url::make_up_get(url_info);
	
	std::string str_post = net_url::make_up_post(url_info,"0");

	// dns ==> ip port
	std::string str_temp;
	str_temp = url_info.get_infomation(UF_SCHEMA);
	std::transform(str_temp.begin(),str_temp.end(),str_temp.begin(),tolower);
	bool bSSL = str_temp == "https";

	// init 
	if ( bSSL )
	{
		net_ssl_ptr = boost::make_shared<net_tcp<net_ssl_base<>>>(io_ptr);

		auto context = net_ssl_base<>::new_context_client(net_ssl_ptr->ec);
		net_ssl_ptr->get_base().socket_ = net_ssl_base<>::new_socket(io_ptr, *context);
	}
	else
	{
		net_ptr = boost::make_shared<net_tcp<net_base<>>>(io_ptr);
	}

	// connect
	if ( bSSL )
	{
		auto it = net_ssl_ptr->yield_dns(yield,
			url_info.get_infomation(UF_HOST),
			url_info.schema2port(url_info.get_infomation(UF_PORT)));
		net_ssl_base<>::yield_connect(yield,net_ssl_ptr->ec,net_ssl_ptr->get_base().get_socket(),it);
	}
	else
	{
		auto it = net_ptr->yield_dns(yield,
			url_info.get_infomation(UF_HOST),
			url_info.schema2port(url_info.get_infomation(UF_PORT)));
		net_base<>::yield_connect(yield,net_ptr->ec,net_ptr->get_base().get_socket(),*it);
	}

	// send
	if ( bSSL )
	{
		net_ssl_ptr->yield_send_hold(yield,str_get);
	}
	else
	{
		net_ptr->yield_send_hold(yield,str_get);
	}

	// recv 
	std::vector<char> v_buff;
	v_buff.resize(2048);
	http_parser_ptr = boost::make_shared<http_work<>>(HTTP_BOTH);
	do{
	if ( bSSL )
	{
		size_t length = net_ssl_ptr->yield_recv(yield,v_buff);
		http_parser_ptr->insert_buff(&v_buff[0],length);
	}
	else
	{
		size_t length = net_ptr->yield_recv(yield,v_buff);
		http_parser_ptr->insert_buff(&v_buff[0],length);
	}
	}while( http_parser_ptr->is_work() );

	// http parser
	std::vector<std::shared_ptr<http_copy_buff<>>> result;
	http_parser_ptr->fetch_result( result);

	for( auto ptr : result )
	{
		if ( !ptr )
			continue;
		auto it = ptr->header.find("Content-Encoding");
		if ( it != ptr->header.end() )
		{
			if (it->second.find("gzip") != std::string::npos)
			{
				std::string str;


				CryptoPP::Gunzip gunzip(new CryptoPP::StringSink(str));

				for ( auto p : ptr->body_buff )
					gunzip.Put(reinterpret_cast<PBYTE>(&(*p)[0]),p->size());

				gunzip.MessageEnd();
				v_buff.clear();
				v_buff.insert(v_buff.end(),str.begin(),str.end());
			}
		}
	}

	if ( http_parser_ptr->is_alive() )
	{
		// send next

		// recv ....
	}












	clear();

}