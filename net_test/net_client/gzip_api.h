#pragma once


// more infomation see http://www.cryptopp.com/wiki/Gzip

#include <gzip.h>

#include <string>
#include <vector>
#include <algorithm>

class gzip_zip
{
public:
	gzip_zip();
	~gzip_zip();
public:
	size_t push(const byte* data, size_t length);

	void push_end();

	template<typename T>
	bool copy_result(T& ret)
	{
		if (result_buff.empty())
			return false;
		ret.resize(result_buff.size());
		std::copy(result_buff.begin(), result_buff.end(), ret.begin());
		return true;
	}
public:
	std::vector<byte> result_buff;
	CryptoPP::Gzip zipper;
};


class gzip_unzip
{
public:
	gzip_unzip();
	~gzip_unzip();
public:
	size_t push(const byte* data, size_t length);

	void push_end();

	template<typename T>
	bool copy_result(T& ret)
	{
		if (result_buff.empty())
			return false;
		ret.resize(result_buff.size());
		std::copy(result_buff.begin(), result_buff.end(), ret.begin());
		return true;
	}
public:
	std::vector<byte> result_buff;
	CryptoPP::Gunzip unner;
};


