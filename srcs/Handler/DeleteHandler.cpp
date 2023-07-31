#include "DeleteHandler.hpp"
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <filesystem>
#include "utils.hpp"
#include <cstdio>

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
Response DeleteHandler::handleRequest(const Request &request)
{
    std::string filename = request.getActualUri();

	if (!isDirectory(filename.c_str()))
	{
		std::cerr << "Error: file not exist." << std::endl;
		return (Response("404"));
	}
	if (remove(filename.c_str()))
	{
		std::cerr << "Error: file not deleted." << std::endl;
		perror("remove");
		return (Response("500"));
	}

    return (Response("200"));
}
