#ifndef LOCATIONCONTEXT_HPP
#define LOCATIONCONTEXT_HPP

#include <string>
#include <vector>
#include <map>

class LocationContext
{
	public:
		LocationContext();
		~LocationContext();
		void setPath(const std::string& path);
		void setAlias(const std::string& alias);
		void setIndex(const std::string& index);
		void addErrorPage(const int status_code, const std::string& error_page);
		const std::string& getPath() const;	
		const std::string& getAlias() const;
		const std::string& getIndex() const;
		const std::map<int, std::string>& getErrorPage() const;
		
	private:
		std::string _path;
		std::string _alias;
		std::string _index;
		std::map<int, std::string> _error_page;

};

#endif
