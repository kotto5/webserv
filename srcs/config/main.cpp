#include "Config.hpp"
#include "ServerContext.hpp"
#include <iostream>

int main()
{
	std::string filepath = "../../conf/error_file/no_http.conf";
	Config config(filepath);
	HTTPContext& http_block = config.getHTTPBlock();

	std::cout << "access_log: " << http_block.getAccessLogFile() << std::endl;
	std::cout << "error_log: " << http_block.getErrorLogFile() << std::endl;
	std::cout << "servers: " << std::endl;
	std::map<int, std::vector<ServerContext> >::const_iterator it = http_block.getServers().begin();
	std::map<int, std::vector<ServerContext> >::const_iterator ite = http_block.getServers().end();
	while (it != ite)
	{
		std::cout << "listen: " << it->first << std::endl;
		std::vector<ServerContext>::const_iterator it2 = it->second.begin();
		std::vector<ServerContext>::const_iterator ite2 = it->second.end();
		while (it2 != ite2)
		{
			std::cout << "server_name: " << it2->getServerName() << std::endl;
			it2++;
		}
		it++;
	}

}