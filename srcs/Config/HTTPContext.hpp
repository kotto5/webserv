#ifndef HTTPCONTEXT_HPP
#define HTTPCONTEXT_HPP

#include "ServerContext.hpp"
#include <string>
#include <map>

class HTTPContext
{
	public:
		HTTPContext();
		HTTPContext(const HTTPContext& other);
		HTTPContext& operator=(const HTTPContext& other);
		~HTTPContext();

		void setAccessLogFile(const std::string& accessLogFile);
		void setErrorLogFile(const std::string& errorLogFile);
		void setClientMaxBodySize(const std::string& clientMaxBodySize);
		void addServerBlock(const ServerContext& server);
		void addDirective(const std::string& directive, const std::string& value,
							const std::string& filepath, int line_number);
		const std::string& getAccessLogFile() const;
		const std::string& getErrorLogFile() const;
		const std::map<std::string, std::vector<ServerContext> >& getServers() const;
		const ServerContext& getServerContext(const std::string& port, const std::string& host) const;
		const std::size_t	&getClientMaxBodySize() const;

	private:
		std::string _accessLogFile;
		std::string _errorLogFile;
		std::size_t _clientMaxBodySize;
		std::map<std::string, std::vector<ServerContext> > _servers;
		std::map<std::string, std::string> _directives;
};

#endif