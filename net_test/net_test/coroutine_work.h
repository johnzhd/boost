#pragma once

#include <boost/coroutine/all.hpp>
#include <boost/bind.hpp>

#include <sstream>
#include <fstream>
#include <iostream>

#include <string>

template<char N>
void read_stream()
{
	std::string data( "This is the first line.\n"
		"This, the second.\n"
		"The third has \"a phrase\"!\n"
		);
	std::istringstream instream(data);
	auto read = [](boost::coroutines::coroutine<std::string>::push_type& sink)
	{
		std::string line;
		while(std::getline(instream,line))
			sink(line);
	};

	boost::coroutines::coroutine<std::string>::pull_type reader(read);
	while ( reader )
	{
		auto str = reader.get();
		reader();
	}

}


template<typename T_LINE_FUNC>
void read_file( std::string filename, T_LINE_FUNC func )
{
	std::ifstream ifs;
	ifs.open(filename.c_str());
	if ( ifs.bad() )
		return ;

	auto read = [&](boost::coroutines::coroutine<std::string>::push_type& sink)
	{
		std::string line;
		std::istream& in(ifs);
		while(std::getline(in,line))
			sink(line);
	};

	boost::coroutines::coroutine<std::string>::pull_type reader(read);
	while ( reader )
	{
		auto str = reader.get();
		func(str);
		reader();
	}
}

class write_file
{
public:
	write_file():sink(boost::bind(&write_file::work,this,_1)){};
	~write_file(){};
protected:
	std::ofstream ofs;
	boost::coroutines::coroutine<std::string>::push_type sink;
	void work( boost::coroutines::coroutine<std::string>::pull_type& source )
	{
		while ( source )
		{
		ofs << source.get() << std::endl;
		source();
		}
		ofs.close();
	}
public:
	bool open(std::string filename)
	{
		ofs.open(filename.c_str());
		return !ofs.bad();
	}

	void write_line( std::string line )
	{
		sink(line);
	}
};

