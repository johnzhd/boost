#pragma once


#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

bool find_file(const boost::filesystem::path & dir_path,         // in this directory,
	const std::string & file_name, // search for this name,
	boost::filesystem::path & path_found);


bool list_files(std::string parent, std::vector<std::string>& files, std::string ex = "");



