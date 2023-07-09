#include "ConfigParser.hpp"
#include "HTTPContext.hpp"
#include "ServerContext.hpp"
#include "LocationContext.hpp"
#include "ConfigError.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>

ConfigParser::ConfigParser(Config& config):
	_config(config),
	_line_number(0)
{
}

ConfigParser::~ConfigParser()
{
}

void ConfigParser::setContextType(ContextType context)
{
	_context_type = context;
}

void ConfigParser::setDirectiveType(const std::string& directive)
{
	if (directive == "http")
		_directive_type = HTTP;
	else if (directive == "access_log")
		_directive_type = ACCESS_LOG;
	else if (directive == "error_log")
		_directive_type = ERROR_LOG;
	else if (directive == "server")
		_directive_type = SERVER;
	else if (directive == "listen")
		_directive_type = LISTEN;
	else if (directive == "server_name")
		_directive_type = SERVER_NAME;
	else if (directive == "location")
		_directive_type = LOCATION;
	else if (directive == "alias")
		_directive_type = ALIAS;
	else if (directive == "index")
		_directive_type = INDEX;
	else if (directive == "error_page")
		_directive_type = ERROR_PAGE;
	else
		_directive_type = UNKNOWN;
}

bool ConfigParser::isInHTTPContext()
{
	return _directive_type == HTTP || _directive_type == ACCESS_LOG
			|| _directive_type == ERROR_LOG || _directive_type == SERVER;
}

bool ConfigParser::isInServerContext()
{
	return _directive_type == LISTEN || _directive_type == SERVER_NAME
			|| _directive_type == LOCATION;
}

bool ConfigParser::isInLocationContext()
{
	return  _directive_type == ALIAS || _directive_type == INDEX
			|| _directive_type == ERROR_PAGE;
}

bool ConfigParser::isAllowedDirective()
{
	if (_context_type == HTTP_CONTEXT)
		return isInHTTPContext();
	else if (_context_type == SERVER_CONTEXT)
		return isInServerContext();
	else if (_context_type == LOCATION_CONTEXT)
		return isInLocationContext();
	return false;
}

void ConfigParser::parseFile(const std::string& filepath)
{
	_filepath = filepath;
	std::ifstream ifs(_filepath.c_str());

	if (!ifs)
	{
		std::cerr << "Open Error" << std::endl;
		exit(EXIT_FAILURE);
	}
	getAndSplitLines(ifs);
	ifs.close();
	parseLines();
}

void ConfigParser::getAndSplitLines(std::ifstream& ifs)
{
	std::string line;
	while (std::getline(ifs, line))
	{
		const std::vector<std::string> words = splitLine(line);
		_lines.push_back(words);
	}
}

std::vector<std::string> ConfigParser::splitLine(const std::string& line)
{
	std::vector<std::string> words;
	size_t start = 0;
	size_t end = 0;

	while (line[start])
	{
		while (isspace(line[start]))
			start++;
		end = start;
		while (isprint(line[end]) && !isspace(line[end])
				&& (line[end] != '{' && line[end] != '}') && line[end] != ';')
			end++;
		if (end > start)
			words.push_back(line.substr(start, end - start));
		if (line[end] == '{' || line[end] == '}' || line[end] == ';')
		{
			words.push_back(line.substr(end, 1));
			end++;
		}
		start = end;
	}
	return words;
}

void ConfigParser::parseLines()
{
	setContextType(HTTP_CONTEXT);
	//parse each line
	for ( ; _line_number < _lines.size(); _line_number++)
	{
		_one_line.clear();
		_one_line = _lines[_line_number];
		if (_one_line.empty())
			continue ;
		if (_one_line[0] == "}")
			break ;
		setDirectiveType(_one_line[0]);
		if (!isAllowedDirective())
		{
			throw ConfigError(NOT_ALLOWED_DIRECTIVE, _one_line[0], _filepath, _line_number + 1);
		}
		else if (_directive_type == HTTP)
			setHTTPContext();
		//else
	}
}

void ConfigParser::setHTTPContext()
{
	_line_number++;
	for ( ; _line_number < _lines.size(); _line_number++)
	{
		_one_line.clear();
		_one_line = _lines[_line_number];
		if (_one_line.empty())
			continue ;
		if (_one_line[0] == "}")
			break ;
		setDirectiveType(_one_line[0]);
		if (!isAllowedDirective())
			throw ConfigError(NOT_ALLOWED_DIRECTIVE, _one_line[0], _filepath, _line_number + 1);
		if (_directive_type == ACCESS_LOG)
			_config.getHTTPBlock().setAccessLogFile(_one_line[1]);
		else if (_directive_type == ERROR_LOG)
			_config.getHTTPBlock().setErrorLogFile(_one_line[1]);
		else if (_directive_type == SERVER)
		{
			ServerContext server_context = getServerContext();
			_config.getHTTPBlock().addServerBlock(server_context);
		}
	}
}

const ServerContext ConfigParser::getServerContext()
{
	ServerContext server_context = ServerContext();

	setContextType(SERVER_CONTEXT);
	_line_number++;
	for ( ; _line_number < _lines.size(); _line_number++)
	{
		_one_line.clear();
		_one_line = _lines[_line_number];
		if (_one_line.empty())
			continue ;
		if (_one_line[0] == "}")
			break ;
		setDirectiveType(_one_line[0]);
		if (!isAllowedDirective())
			throw ConfigError(NOT_ALLOWED_DIRECTIVE, _one_line[0], _filepath, _line_number + 1);
		if (_directive_type == LISTEN)
			server_context.setListen(_one_line[1]);
		else if (_directive_type == SERVER_NAME)
			server_context.setServerName(_one_line[1]);
		else if (_directive_type == LOCATION)
		{
			LocationContext location_context = getLocationContext();
			server_context.addLocationBlock(location_context);
		}
	}
	return server_context;
}

const LocationContext ConfigParser::getLocationContext()
{
	LocationContext location_context = LocationContext();

	setContextType(LOCATION_CONTEXT);
	location_context.addDirective("path", _one_line[1]);
	_line_number++;
	for ( ; _line_number < _lines.size(); _line_number++)
	{
		_one_line.clear();
		_one_line = _lines[_line_number];
		if (_one_line.empty())
			continue ;
		if (_one_line[0] == "}")
			break ;
		setDirectiveType(_one_line[0]);
		if (!isAllowedDirective())
			throw ConfigError(NOT_ALLOWED_DIRECTIVE, _one_line[0], _filepath, _line_number + 1);
		else if (_directive_type == ERROR_PAGE)
			location_context.addDirective(_one_line[1], _one_line[2]);
		else
			location_context.addDirective(_one_line[0], _one_line[1]);
	}
	return location_context;
}
