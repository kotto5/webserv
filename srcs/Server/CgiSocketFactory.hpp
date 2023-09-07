#ifndef CGISOCKETFACTORY_HPP
#define CGISOCKETFACTORY_HPP

#include "CgiSocketFactory.hpp"
#include "CgiResponse.hpp"
#include "Socket.hpp"
#include "Request.hpp"

class CgiSocketFactory
{
private:
    CgiSocketFactory();
    ~CgiSocketFactory();
    const CgiSocketFactory &operator=(const CgiSocketFactory &other);
    enum {
        S_CHILD = 0,
        S_PARENT = 1,
    };
    static std::vector<char *> *createEnvs(const Request &request);
    static int runCgi(const Request &request, int pipes[2]);
    static CgiSocket *create(const Request &request, ClSocket *clientSocket);
    static CgiSocket *create(const CgiResponse &cgiResponse, ClSocket *clientSocket);
public:
    static CgiSocket *create(const HttpMessage &message, ClSocket *clientSocket);
};

#endif