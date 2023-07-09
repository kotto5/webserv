#include "Config.hpp"
#include "ServerContext.hpp"
#include <iostream>

int main()
{
	try
	{
		std::string filepath = "../../conf/error_file/unknown_directive.conf";
		Config config(filepath);

		std::string accessLogFile = Config::getInstance()->getHTTPBlock().getAccessLogFile();
		std::cout << accessLogFile << std::endl;
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}
}