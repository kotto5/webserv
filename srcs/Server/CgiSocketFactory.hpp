#ifndef CGISOCKETFACTORY_HPP
#define CGISOCKETFACTORY_HPP

#include "CgiSocketFactory.hpp"
#include "utils.hpp"
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
public:
    static CgiSocket *create(const Request &request, ClSocket *clientSocket);
};

#endif