#ifndef LOCATIONCONTEXT_HPP
#define LOCATIONCONTEXT_HPP

#include <string>
#include <vector>
#include <map>
#include <vector>

class LocationContext
{
	public:
		LocationContext();
		LocationContext(const LocationContext& other);
		~LocationContext();
		LocationContext& operator=(const LocationContext& other);
		// void addErrorPage(const std::string& status_code, const std::string& error_page);
		void addDirective(const std::string& directive, const std::string& value,
							const std::string& filepath, int line_number);
		std::string	getDirective(const std::string& directive) const;
		void setAllowedMethods(const std::vector<std::string>& allowedMethods);
		const std::vector<std::string>& getAllowedMethods() const;

	private:
		std::map<std::string, std::string> _directives;
		std::vector<std::string> _allowedMethods;

};

#endif
