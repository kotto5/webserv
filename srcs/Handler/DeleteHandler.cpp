#include "DeleteHandler.hpp"
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <filesystem>
#include "utils.hpp"
#include <cstdio>
#include "Logger.hpp"
#include "RequestException.hpp"

// Constructors
DeleteHandler::DeleteHandler()
{
	// 規定値を204に設定
	this->_status = "204";
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
		Logger::instance()->writeErrorLog(ErrorCode::DELETE_FILE_NOT_EXIST, "File not found");
		throw RequestException("404");
	}
	if (remove(filename.c_str()))
	{
		Logger::instance()->writeErrorLog(ErrorCode::DELETE_FILE_NO_PERMISSION, "Permission denied");
		throw RequestException("500");
	}

    return (new Response(_status));
}
