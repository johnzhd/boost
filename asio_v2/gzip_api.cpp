#include "stdafx.h"
#include "gzip_api.h"



gzip_zip::gzip_zip()
{

}

gzip_zip::~gzip_zip()
{

}

size_t gzip_zip::push(const byte * data, size_t length)
{
	return zipper.Put(data, length);
}

void gzip_zip::push_end()
{
	zipper.MessageEnd();
	size_t size = zipper.MaxRetrievable();
	if (size)
	{
		result_buff.resize(size);
		zipper.Get(&result_buff[0], result_buff.size());
	}
	else
	{
		result_buff.clear();
	}
}

gzip_unzip::gzip_unzip()
{
}

gzip_unzip::~gzip_unzip() {}

size_t gzip_unzip::push(const byte * data, size_t length)
{
	return unner.Put(data, length);
}

void gzip_unzip::push_end()
{
	unner.MessageEnd();
	size_t size = unner.MaxRetrievable();
	if (size)
	{
		result_buff.resize(size);
		unner.Get(&result_buff[0], result_buff.size());
	}
	else
	{
		result_buff.clear();
	}
}
