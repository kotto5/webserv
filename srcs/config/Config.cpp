#include "Config.hpp"
#include <iostream>

//default constructor
Config::Config()
{
}

//destructor
Config::~Config()
{
}

Config* Config::get_instance()
{
	if (_instance == NULL)
		_instance = new Config();
	return _instance;
}

