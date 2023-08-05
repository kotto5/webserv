#ifndef AUTOINDEX_HPP
# define AUTOINDEX_HPP

# include <iostream>
# include <string>
# include <vector>
# include "FileInfo.hpp"
# include "Request.hpp"

class Autoindex
{
	public:

		// Constructors
		Autoindex(const Request &request);
		Autoindex(const Autoindex &other);

		// Destructor
		~Autoindex();

		// Operators
		Autoindex & operator=(const Autoindex &rhs);

		void parseDirectory();
		std::string generateAutoindex();

	private:
		std::vector<FileInfo> _fileInfo;
		std::string _uri;
		std::string _actualPath;
		Autoindex();
		std::string createElement(const std::string &content, const std::string &tag, const std::string &attr = "");
		std::string convertTimeToDate(time_t time);
		std::string formatSize(long long byte);
		std::string getIcon(const FileInfo &file);
};

#endif