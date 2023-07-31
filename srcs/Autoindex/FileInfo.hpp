#ifndef FILEINFO_HPP
# define FILEINFO_HPP

# include <iostream>

class FileInfo
{
public:
	FileInfo(const std::string &name, const std::string& path, time_t lastModified, off_t fileSize);
	FileInfo(const FileInfo &other);
	~FileInfo();
	FileInfo &operator=(const FileInfo &other);

	std::string fileName;
	std::string filepath;
	time_t lastModified;
	off_t fileSize;
};

#endif