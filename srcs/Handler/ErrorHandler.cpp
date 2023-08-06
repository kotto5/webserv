#include "ErrorHandler.hpp"
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <filesystem>
#include "utils.hpp"
#include <cstdio>
#include "Logger.hpp"

// Constructors
ErrorHandler::ErrorHandler()
{
	// 規定値を404に設定
	this->_status = "404";
}

ErrorHandler::ErrorHandler(const ErrorHandler &other)
{
	*this = other;
}

// Destructor
ErrorHandler::~ErrorHandler() {}

// Operators
ErrorHandler &ErrorHandler::operator=(const ErrorHandler &rhs)
{
	if (this != &rhs)
	{
		this->_status = rhs._status;
	}
	return *this;
}

/**
 * @brief エラーの処理を行う
 *
 * @param request リクエスト
 * @return Response レスポンス
 */
Response	*ErrorHandler::handleRequest(const Request &request)
{
	std::string filename = request.getActualUri();

	//　エラーページが参照できない場合はデフォルトの内容を返す
	if (!pathExist(filename.c_str()))
	{
		return (new Response("404"));
	}
	return (new Response("204"));
}
