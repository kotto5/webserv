#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <string>

class ConfigParser
{
    public:
        ConfigParser();
        ~ConfigParser();
        void parseFile(const std::string &filepath);

    private:
        Config& _config;
        std::string _filepath;
        std::vector<std::vector<std::string> > _lines;
        
};

#endif