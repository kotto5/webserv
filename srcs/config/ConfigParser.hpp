#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include "Config.hpp"
#include "HTTPContext.hpp"
#include "LocationContext.hpp"
#include "ServerContext.hpp"
#include <string>
#include <vector>

class ConfigParser
{
public:
	ConfigParser(Config &config);
	~ConfigParser();
	void parseFile(const std::string &filepath);
	void getAndSplitLines(std::ifstream &ifs);
	std::vector<std::string> splitLine(const std::string &line);
	void parseLines();
	void setHTTPContext();
	const ServerContext getServerContext();
	const LocationContext getLocationContext();
	void setDirectiveType(const std::string &directive);

	enum DirectiveType
	{
		HTTP,
		ACCESS_LOG,
		ERROR_LOG,
		SERVER,
		LISTEN,
		SERVER_NAME,
		LOCATION,
		ALIAS,
		INDEX,
		ERROR_PAGE
	};

private:
	Config &_config;
	size_t _line_number;
	std::string _filepath;
	std::vector<std::vector<std::string> > _lines;
	std::vector<std::string> _one_line;
	DirectiveType _directive_type;
	int stoi(const std::string &str);
};

#endif