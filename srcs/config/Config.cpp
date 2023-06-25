#include "Config.hpp"
#include <iostream>
#include <fstream>

//default constructor
Config::Config()
{
}

//destructor
Config::~Config()
{
}

//read config file
void Config::readFile(const std::string &filepath)
{
	std::ifstream ifs(filepath.c_str());
	if (ifs.fail())
		throw std::runtime_error("failed to open config file");
	std::string line;
	while (std::getline(ifs, line))
	{
		if (line.empty())
			continue;
		if (line[0] == '#')
			continue;
		if (line.find("server") != std::string::npos)
		{
			ServerConfig server_config;
			server_config.readServerConfig(ifs);
			addServer(server_config);
		}
	}
}

//add server config
void Config::addServer(const ServerConfig &server_config)
{
	server_.insert(std::make_pair(server_config.port(), server_config));
}

//get server config
const std::map<int, std::vector<ServerConfig> > &Config::server() const
{
	return server_;
}
