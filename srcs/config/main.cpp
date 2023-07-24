#include "Config.hpp"
#include "ConfigError.hpp"
#include <iostream>

int main()
{
	try {
		//create test
		Config config("../../conf/error_file/duplicate.conf");

		//test getHTTPBlock
		HTTPContext& http_block = Config::getInstance()->getHTTPBlock();
		std::cout << "accessLogFile: " << http_block.getAccessLogFile() << std::endl;
	}
	catch (ConfigError& e) {
		std::cout << e.what() << std::endl;
	}
}