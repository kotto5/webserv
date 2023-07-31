#include "FileInfo.hpp"

FileInfo::FileInfo(const std::string &name, const std::string& path, time_t lastModified, off_t fileSize)
	: fileName(name), filepath(path), lastModified(lastModified), fileSize(fileSize)
{
}

FileInfo::FileInfo(const FileInfo &other)
{
	*this = other;
}

FileInfo::~FileInfo()
{
}

FileInfo &FileInfo::operator=(const FileInfo &rhs)
{
	if (this != &rhs)
	{
		fileName = rhs.fileName;
		filepath = rhs.filepath;
		lastModified = rhs.lastModified;
		fileSize = rhs.fileSize;
	}
	return *this;
}