#ifndef AUTOINDEX_HPP
# define AUTOINDEX_HPP

# include <iostream>
# include <string>
# include <vector>
# include "FileInfo.hpp"

class Autoindex
{
	public:

		// Constructors
		Autoindex(const std::string& path);
		Autoindex(const Autoindex &other);

		// Destructor
		~Autoindex();

		// Operators
		Autoindex & operator=(const Autoindex &rhs);

		void parseDirectory();
		std::string generateAutoindex();

	private:
		std::vector<FileInfo> _fileInfo;
		const std::string _path;
		Autoindex();
		std::string createElement(const std::string &content, const std::string &tag);
		std::string createHyperlink(const std::string &content, const std::string &url);
		std::string convertTimeToDate(time_t time);


};

#endif