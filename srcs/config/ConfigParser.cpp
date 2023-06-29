#include "ConfigParser.hpp"
#include "HTTPContext.hpp"
#include "ServerContext.hpp"
#include "LocationContext.hpp"
#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <cstdlib>

ConfigParser::ConfigParser(Config& config):
	_line_number(0),
	_config(config)
{
}

ConfigParser::~ConfigParser()
{
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
	//else
		//exception
}

void ConfigParser::parseFile(const std::string& filepath)
{
	_filepath = filepath;
	std::ifstream ifs(_filepath.c_str());

	if (!ifs)
	{
		std::cerr << "Open Error" << std::endl;
		exit(1);
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
		// error handling

		if (_directive_type == HTTP)
			setHTTPContext();
		else
			//exception
			;
	}
	
}

void ConfigParser::setHTTPContext()
{
	HTTPContext http_context = HTTPContext();

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
		if (_directive_type == ACCESS_LOG)
			http_context.setAccessLogFile(_one_line[1]);
		else if (_directive_type == ERROR_LOG)
			http_context.setErrorLogFile(_one_line[1]);
		else if (_directive_type == SERVER)
			setServerContext(http_context);
	}

}

void ConfigParser::setServerContext(HTTPContext& http_context)
{
	ServerContext server_context = ServerContext();

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
		if (_directive_type == LISTEN)
			server_context.setListen(stoi(_one_line[1]));
		else if (_directive_type == SERVER_NAME)
			server_context.setServerName(_one_line[1]);
		else if (_directive_type == LOCATION)
			setLocationContext();
	}
	//http_context.addServerContext(server_context);
}

void ConfigParser::setLocationContext()
{
	LocationContext location_context = LocationContext();

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
		if (_directive_type == ALIAS)
			location_context.setAlias(_one_line[1]);
		else if (_directive_type == INDEX)
			location_context.setIndex(_one_line[1]);
		else if (_directive_type == ERROR_PAGE)
			location_context.addErrorPage(stoi(_one_line[1]), _one_line[2]);
	}
	//server_context.addLocationContext(location_context);
}

const int ConfigParser::stoi(const std::string& str)
{
	int num = 0;
	int sign = 1;
	int i = 0;

	if (str[i] == '-')
	{
		sign = -1;
		i++;
	}
	while (str[i])
	{
		num = num * 10 + (str[i] - '0');
		i++;
	}
	return (num * sign);
}
