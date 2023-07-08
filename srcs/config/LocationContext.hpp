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
		void addErrorPage(const std::string& status_code, const std::string& error_page);
		void addDirective(const std::string& directive, const std::string& value);
		const std::map<std::string, std::string>& getErrorPage() const;
		
	private:
		std::string _path;
		std::string _alias;
		std::string _index;
		std::map<std::string, std::string> _error_page;
		std::map<std::string, std::string> _directives;

};

#endif
