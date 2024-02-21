#ifndef FILE_HPP
#define FILE_HPP

#include "common.hpp"

class File
{
	private:
		File();

	public:
		std::string	filename;
		std::string path;
		std::string nicksender;
		std::string nickreceiver;
		
		File(std::string n, std::string p, std::string s, std::string r);
		File(File const & base);
		~File();

		File & operator=(File const & base);
};

#endif