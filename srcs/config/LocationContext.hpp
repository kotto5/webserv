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
		//addDirective
		void addErrorPage(const std::string& status_code, const std::string& error_page);
		const std::string& getPath() const;	
		const std::string& getAlias() const;
		const std::string& getIndex() const;
		const std::map<std::string, std::string>& getErrorPage() const;
		//getDirective(const std::string& directive);
		
	private:
		//std::map<std::string, std::string> _directives;
		std::string _path;
		std::string _alias;
		std::string _index;
		std::map<std::string, std::string> _error_page;

};

#endif
