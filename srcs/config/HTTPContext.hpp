#ifndef HTTPCONTEXT_HPP
#define HTTPCONTEXT_HPP

#include "ServerContext.hpp"
#include <string>
#include <map>

class HTTPContext
{
	public:
		HTTPContext();
		~HTTPContext();
		void setAccessLogFile(const std::string& accessLogFile);
		void setErrorLogFile(const std::string& errorLogFile);
		const std::string& getAccessLogFile() const;
		const std::string& getErrorLogFile() const;
		void addServerBlock(const ServerContext& server);
		const std::map<std::string, std::vector<ServerContext> >& getServers() const;
		const ServerContext& getServerContexts(const std::string& port, const std::string& host) const;

	private:
		std::string _accessLogFile;
		std::string _errorLogFile;
		std::map<std::string, std::vector<ServerContext> > _servers;
};

#endif