#ifndef CONFIG_PARSER_HPP
#define CONFIG_PARSER_HPP

#include <string>

class ConfigParser
{
    public:
        ConfigParser();
        ~ConfigParser();
        void parseFile(const std::string &filepath);
        void getAndSplitLines(const std::string &filepath);
        std::vector<std::string> splitLine(const std::string &line);
        void parseLines(const std::string &line);
        void parseHTTPContext(const std::string &line);
        void parseServerContext(const std::string &line);
        void parseLocationContext(const std::string &line);

    private:
        Config& _config;
        std::string _filepath;
        std::vector<std::vector<std::string> > _lines;
        
};

#endif