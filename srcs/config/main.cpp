#include "Config.hpp"
#include "ServerContext.hpp"
#include <iostream>

int main()
{
	try
	{
		std::string filepath = "../../conf/default.conf";
		Config config(filepath);

		std::cout << "------------------------------" << std::endl;
		std::string accessLogFile 
			= Config::getInstance()
					->getHTTPBlock()
					.getAccessLogFile();
		std::cout << accessLogFile << std::endl;
		std::cout << "------------------------------" << std::endl;
		std::string error_page 
			= Config::getInstance()
					->getHTTPBlock()
					.getServerContext()
					.getLocationBlock(0)
					.getErrorPage(404);
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}