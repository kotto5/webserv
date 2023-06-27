#include "ServerContext.hpp"

ServerContext::ServerContext():
	_listen()
	_server_name()
	// _index()
	// _error_page()
{
}

ServerContext::~ServerContext()
{
}

void ServerContext::setListen(int listen)
{
	_listen = listen;
}

void ServerContext::setServerName(const std::string &server_name)
{
	_server_name = server_name;
}

// void ServerContext::addIndex(const std::string &index)
// {
// 	_index.push_back(index);
// }	

// void ServerContext::addErrorPage(int error_code, const std::string &error_page)
// {
// 	_error_page.insert(std::make_pair(error_code, error_page));
// }

int ServerContext::getListen() const
{
	return _listen;
}

const std::string& ServerContext::getServerName() const
{
	return _server_name;
}

// const std::vector<std::string>& ServerContext::getIndex() const
// {
// 	return _index;
// }

// const std::map<int, std::string>& ServerContext::getErrorPage() const
// {
// 	return _error_page;
// }
