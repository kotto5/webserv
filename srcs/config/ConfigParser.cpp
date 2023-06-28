#include "ConfigParser.hpp"
#include "HTTPContext.hpp"
#include "ServerContext.hpp"
#include "LocationContext.hpp"
#include <fstream>
#include <vector>
#include <string>

ConfigParser::ConfigParser(Config& config):
	_config(config)
{
}

ConfigParser::~ConfigParser()
{
}

void ConfigParser::parseFile(const std::string& filepath)
{
	_filepath = filepath;
	std::ifstream ifs(filepath);

	if (!ifs)
	{
		//exception
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
	std::vector<std::vector<std::string> >::iterator it = _lines.begin();
	std::vector<std::vector<std::string> >::iterator ite = _lines.end();

	while (it != ite)
	{
		if ((*it).size() == 0)
		{
			it++;
			continue;
		}
		if ((*it).at(0) == "http")
		{
			parseHTTPContext(it);
		}
		else
		{
			//exception
		}
		it++;
		if ((*it).size() == 0)
		{
			it++;
			continue;
		}
		if ((*it).at(0) == "server")
		{
			parseServerContext(it);
		}
		else
		{
			//exception
		}
		it++;

	}
}

void ConfigParser::parseHTTPContext(std::vector<std::vector<std::string> >::iterator& it)
{
	HTTPContext http_context = HTTPContext();

	if ((*it).size() != 2 || (*it).at(1) != "{")
	{
		//exception
	}
	it++;
	while (it != _lines.end())
	{
		if ((*it).size() == 0)
		{
			it++;
			continue;
		}
		if ((*it).at(0) == "access_log")
		{
			http_context.setAccessLogFile((*it).at(1));
		}
		it++;
		if ((*it).size() == 0)
		{
			it++;
			continue;
		}
		if ((*it).at(0) == "error_log")
		{
			http_context.setErrorLogFile((*it).at(1));
		}
	}
}

void ConfigParser::parseServerContext(std::vector<std::vector<std::string> >::iterator& it)
{
	ServerContext server_context = ServerContext();

	if ((*it).size() != 2 || (*it).at(1) != "{")
	{
		//exception
	}
	it++;
	while (it != _lines.end())
	{
		if ((*it).size() == 0)
		{
			it++;
			continue;
		}
		if ((*it).at(0) == "listen")
		{
			server_context.setListen((*it).at(1));
		}
		else if ((*it).at(0) == "server_name")
		{
			server_context.setServerName((*it).at(1));
		}
		else if ((*it).at(0) == "location")
		{
			parseLocationContext(it);
		}
		else
		{
			//exception
		}
		it++;
	}
	_config.a
}

void ConfigParser::parseLocationContext(std::vector<std::vector<std::string> >::iterator& it)
{
	LocationContext location_context = LocationContext();

	if ((*it).size() != 2 || (*it).at(1) != "{")
	{
		//exception
	}
	it++;
	while (_lines.end())
	{
		if ((*it).size() == 0)
		{
			it++;
			continue;
		}
		if (*it).at(0) == 
	}