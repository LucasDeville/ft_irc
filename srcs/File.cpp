#include "File.hpp"

File::File() {};
File::File(std::string n, std::string p, std::string s, std::string r): filename(n), path(p), nicksender(s), nickreceiver(r) {};
File::File(File const & base) {*this = base;};
File::~File() {};

File & File::operator=( const File &base )
{
	if (this != &base)
	{
		this->filename = base.filename;
		this->path = base.path;
		this->nicksender = base.nicksender;
		this->nickreceiver = base.nickreceiver;
	}
	return (*this);
};