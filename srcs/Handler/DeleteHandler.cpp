#include "DeleteHandler.hpp"
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <filesystem>
#include "utils.hpp"
#include <cstdio>
#include "Logger.hpp"

// Constructors
DeleteHandler::DeleteHandler()
{
	// 規定値を200に設定
	this->_status = "200";
}

DeleteHandler::DeleteHandler(const DeleteHandler &other)
{
	*this = other;
}

// Destructor
DeleteHandler::~DeleteHandler() {}

// Operators
DeleteHandler &DeleteHandler::operator=(const DeleteHandler &rhs)
{
	if (this != &rhs)
	{
		this->_status = rhs._status;
	}
	return *this;
}

/**
 * @brief Deleteメソッドの処理を行う
 *
 * @param request リクエスト
 * @return Response レスポンス
 */
Response	*DeleteHandler::handleRequest(const Request &request)
{
    std::string filename = request.getActualUri();

	if (!pathExist(filename.c_str()))
	{
		std::cerr << "Error: file not exist." << std::endl;
		return (new Response("404"));
	}
	if (remove(filename.c_str()))
	{
		std::cerr << "Error: file not deleted." << std::endl;
		perror("remove");
		return (new Response("500"));
	}

    return (new Response("200"));
}
