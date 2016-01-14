#pragma once


#include <string>
#include <sstream>
#include <boost/asio/ip/tcp.hpp>



#include <base64.h>
#include <gzip.h>

#include <assert.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>



std::string cast_ip(unsigned int ip);

unsigned int cast_ip(std::string ip);

boost::asio::ip::tcp::endpoint cast_endpoint(unsigned long address, unsigned short port);

boost::asio::ip::tcp::endpoint cast_endpoint(std::string address, std::string port);

boost::asio::ip::tcp::endpoint cast_endpoint(std::string address, unsigned short port);

boost::asio::ip::tcp::endpoint cast_endpoint(unsigned long address, std::string port);






std::string cast_json(const boost::property_tree::ptree& p);

std::wstring cast_json(const boost::property_tree::wptree& p);

boost::property_tree::ptree cast_json(const std::string& src);

boost::property_tree::wptree cast_json(const std::wstring& src);




template<typename WT, typename RT>
class crypto_base
{
public:
	crypto_base() {};
	~crypto_base() {};
public:
	RT result;
	WT work_opt;
protected:
	size_t step = 0;
public:

	template<typename IT>
	size_t push(IT begin, IT end)
	{
		size_t n = 0;

		assert(step < 2);
		if (step >= 2)
			return 0;

		while (begin != end)
		{
			if (0 == work_opt.Put(*begin++))
				break;
			n++;
		}

		if (n > 0)
			step = 1;

		return n;
	}

	size_t push(const byte* data, size_t length)
	{
		assert(step < 2);
		if (step >= 2)
			return 0;

		if (length > 0)
			step = 1;
		return work_opt.Put(data, length);
	}

	void push_end()
	{
		assert(step == 1);
		if (step != 1)
			return;

		step = 2;

		work_opt.MessageEnd();
		size_t size = work_opt.MaxRetrievable();
		if (size)
		{
			result.resize(size);
			work_opt.Get((byte*)result.data(), result.size());
		}
		else
		{
			result.clear();
		}
	}

	template<typename T>
	bool copy_result(T& ret)
	{
		if (result.empty())
			return false;
		ret.resize(result.size());
		std::copy(result.begin(), result.end(), ret.begin());
		return true;
	}

	template<>
	bool copy_result<RT>(RT& ret)
	{
		if (result.empty())
			return false;
		result.swap(ret);
		result.clear();

		return true;
	}
};



typedef crypto_base<CryptoPP::Gzip, std::vector<char>> gzip_zip;
typedef crypto_base<CryptoPP::Gunzip, std::vector<char>> gzip_unzip;
typedef crypto_base<CryptoPP::Base64Encoder, std::string> encode_base64;
typedef crypto_base<CryptoPP::Base64Decoder, std::string> decode_base64;



