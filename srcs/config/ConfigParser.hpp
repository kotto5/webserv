#ifndef CONFIGPARSER_HPP
#define CONFIGPARSER_HPP

#include <string>
#include <vector>
#include "Config.hpp"

class ConfigParser
{
    public:
        ConfigParser(Config& config);
        ~ConfigParser();
        void parseFile(const std::string &filepath);
        void getAndSplitLines(std::ifstream& ifs);
        std::vector<std::string> splitLine(const std::string &line);
        void parseLines();
        void parseHTTPContext(std::vector<std::vector<std::string> >::iterator& it);
        void parseServerContext(std::vector<std::vector<std::string> >::iterator& it);
        void parseLocationContext(std::vector<std::vector<std::string> >::iterator& it);

    private:
        Config& _config;
        std::string _filepath;
        std::vector<std::vector<std::string> > _lines;
        
};

#endif