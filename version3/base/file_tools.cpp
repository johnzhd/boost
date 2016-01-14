#include "stdafx.h"
#include "file_tools.hpp"

bool find_file(const boost::filesystem::path & dir_path, const std::string & file_name, boost::filesystem::path & path_found)            // placing path here if found
{
	if (!boost::filesystem::exists(dir_path)) return false;
	boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
	for (boost::filesystem::directory_iterator itr(dir_path);
	itr != end_itr;
		++itr)
	{
		if (boost::filesystem::is_directory(itr->status()))
		{
			if (find_file(itr->path(), file_name, path_found)) return true;
		}
		else if (itr->path().stem() == file_name) // see below
		{
			path_found = itr->path();
			return true;
		}
	}
	return false;
}

bool list_files(std::string parent, std::vector<std::string>& files, std::string ex)
{
	boost::filesystem::path p(parent);
	if (!boost::filesystem::exists(p))
		return false;

	boost::to_upper(ex);
	std::string str;
	boost::filesystem::directory_iterator it_end;
	for (boost::filesystem::directory_iterator it(p); it != it_end; it++)
	{
		if (boost::filesystem::is_regular_file(it->path()))
		{
			str = it->path().extension().string();
			boost::to_upper(str);
			if (ex.empty() || str == ex)
			{
				files.push_back(it->path().string());
			}
		}
		// else if (boost::filesystem::is_directory(it->status()))
		//{
		//	continue;
		//}
	}
	return true;
}
