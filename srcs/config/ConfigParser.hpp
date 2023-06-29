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
        void setDirectiveType(const std::string& directive);
        void parseFile(const std::string &filepath);
        void getAndSplitLines(std::ifstream& ifs);
        std::vector<std::string> splitLine(const std::string& line);
        void parseLines();
        void ConfigParser::setHTTPContext();
        void ConfigParser::setServerContext();
        void ConfigParser::setLocationContext();

        enum DirectiveType
        {
            HTTP,
            ACCESS_LOG,
            ERROR_LOG,
            SERVER,
            LISTEN,
            SERVER_NAME,
            LOCATION,
            ALIAS,
            INDEX,
            ERROR_PAGE
        };

    private:
        size_t _line_number;
        Config& _config;
        std::string _filepath;
        std::vector<std::vector<std::string> > _lines;
        std::vector<std::string> _one_line;
        DirectiveType _directive_type;
};

#endif