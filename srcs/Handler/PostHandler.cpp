#include "PostHandler.hpp"
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <filesystem>
#include "utils.hpp"
#include "Logger.hpp"
#include "Response.hpp"
#include "RequestException.hpp"
#include <stdio.h>
#include <stdint.h>

// Constructors
PostHandler::PostHandler()
{
	// 規定値を201に設定
	this->_status = "201";
}

PostHandler::PostHandler(const PostHandler &other)
{
	*this = other;
}

// Destructor
PostHandler::~PostHandler() {}

// Operators
PostHandler &PostHandler::operator=(const PostHandler &rhs)
{
	if (this != &rhs)
	{
		this->_status = rhs._status;
	}
	return *this;
}

std::string	PostHandler::getUniqueFileNameWithPrefix(const std::string &dir, const std::string &file)
{
	std::string			fileNameWithPrefix = file;
	try {
		std::size_t prefix = 0;
		while (prefix < SIZE_MAX)
		{
			std::stringstream	ss;
			if (!pathExist((dir + fileNameWithPrefix).c_str()))
				return (fileNameWithPrefix);
			ss << prefix << file;
			fileNameWithPrefix = ss.str();
			prefix++;
		}
		return ("");
	}
	catch (const std::exception &e)
	{
		return ("");
	}
}

/**
 * @brief POSTメソッドの処理を行う
 *
 * @param request リクエスト
 * @return Response レスポンス
 */
HttpMessage *PostHandler::handleRequest(const Request &request, const ServerContext &serverContext)
{
	std::string::size_type pos = request.getActualUri().find_last_of('/');
    std::string dir = request.getActualUri().substr(0, pos + 1);
	std::string actualFileName;

	if (!pathExist(dir.c_str())) // ディレクトリが存在しない
		return (handleError("404", serverContext));

	std::string originFileName = request.getActualUri().substr(pos + 1);
	actualFileName = getUniqueFileNameWithPrefix(dir, originFileName);
	if (actualFileName.empty())
		return (handleError("500", serverContext));

	std::ofstream ofs((dir + actualFileName).c_str(), std::ios::out);
	if (!ofs)
	{
		if (errno == EACCES)
		{
			// ファイルにアクセスできない
			Logger::instance()->writeErrorLog(ErrorCode::POST_FILE_ACCESS, "j", &request);
			return (handleError("403", serverContext));
		}
		Logger::instance()->writeErrorLog(ErrorCode::POST_FILE_OPEN, "", &request);
		return (handleError("500", serverContext));
    }
    ofs << request.getBody();
    ofs.close();

	// レスポンスヘッダーの作成
	std::map<std::string, std::string> headers;
	pos = request.getUri().find_last_of('/');
    std::string uriDir = request.getUri().substr(0, pos + 1);
	HttpMessage::setHeader(headers, "Location", uriDir + actualFileName);
	HttpMessage::setHeader(headers, "content-type", Response::getMimeType(request.getActualUri()));

    return (new Response(_status, headers, ""));
}
