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
		ServerContext server 
			= Config::getInstance()
					->getHTTPBlock()
					.getServerContext("80", "webserve1");
		std::cout << server.getServerName() << std::endl;
		std::cout << "------------------------------" << std::endl;
		// std::string error_page
		// 	= server.getLocationContext("/error_page/")
		// 			.getDirective("404");
		// std::cout << error_page << std::endl;
		std::cout << "------------------------------" << std::endl;
		std::string error_alias
			= server.getLocationContext("/error_page/")
					.getDirective("alias");
		std::cout << error_alias << std::endl;
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}