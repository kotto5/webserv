#include "Config.hpp"
#include "ServerContext.hpp"
#include <iostream>

int main()
{
	std::string filepath = "../../conf/default.conf";
	Config config;
	config.readFile(filepath);
	const std::map<int, std::vector<ServerContext> >& servers = config.getServers();
	std::cout << servers.size() << std::endl;
	std::cout << "-------TEST 1-------" << std::endl;
	//define servers iterator
	std::map<int, std::vector<ServerContext> >::const_iterator servers_it;
	//iterate through servers
	for (servers_it = servers.begin(); servers_it != servers.end(); servers_it++)
	{
		//define server vector
		const std::vector<ServerContext>& server_vector = servers_it->second;
		//define server vector iterator
		std::vector<ServerContext>::const_iterator server_vector_it;
		//iterate through server vector
		for (server_vector_it = server_vector.begin(); server_vector_it != server_vector.end(); server_vector_it++)
		{
			//define server
			const ServerContext& server = *server_vector_it;
			//print server
			std::cout << "listen: " << server.getListen() << std::endl;
			std::cout << "server_name: " << server.getServerName() << std::endl;
		}
	}
}