#ifndef SERVERCONTEXT_HPP
#define SERVERCONTEXT_HPP

#include <string>
#include <map>
#include <vector>

class ServerContext
{
    public:
        ServerContext();
        ~ServerContext();
        void setListen(int listen);
        void setServerName(const std::string& server_name);
        // void addIndex(const std::string& index);
        // void addErrorPage(const std::string& error_page);
        int getListen() const;
        const std::string& getServerName() const;
        // const std::vector<std::string>& getIndex() const;
        // const std::map<int, std::string>& getErrorPage() const;

    
    private:
        int _listen;
        std::string _server_name;
//         std::vector<std::string> _index;
//         std::map<int, std::string> _error_page;
};

#endif