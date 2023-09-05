#include "PostHandler.hpp"
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <filesystem>
#include "utils.hpp"
#include "Logger.hpp"
#include "Response.hpp"
#include "RequestException.hpp"
#include <cstdint>

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

/**
 * @brief POSTメソッドの処理を行う
 *
 * @param request リクエスト
 * @return Response レスポンス
 */

HttpMessage *PostHandler::handleRequest(const Request &request, const ServerContext &serverContext)
{
	(void)serverContext;
	std::string::size_type pos = request.getActualUri().find_last_of('/');
    std::string filedir = request.getActualUri().substr(0, pos + 1);
    std::string filename = request.getActualUri().substr(pos + 1);
	std::string tmp = filename;

	// ファイル名が重複していた場合は連番をつける
	std::size_t i = 0;
	while (i < SIZE_MAX)
	{
		if (!pathExist((filedir + tmp).c_str()))
		{
			filename = tmp;
			break;
		}
		tmp = std::to_string(i) + filename;
		i++;
	}
	if (i == SIZE_MAX)
	{
		// ファイル数が上限に達した
		Logger::instance()->writeErrorLog(ErrorCode::POST_INDEX_FULL, "", &request);
		return (handleError("500", serverContext));
	}
	if (!pathExist(filedir.c_str()))
	{
		// ディレクトリが存在しない
		Logger::instance()->writeErrorLog(ErrorCode::POST_NOT_EXISTS, "", &request);
		return (handleError("404", serverContext));
	}
	std::ofstream ofs(filedir + filename, std::ios::out);
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
	std::string body = request.getBody();
    ofs << body;
    ofs.close();

	// レスポンスヘッダーの作成
	std::map<std::string, std::string> headers;
	pos = request.getUri().find_last_of('/');
    std::string uridir = request.getUri().substr(0, pos + 1);
	HttpMessage::setHeader(headers, "Location", uridir + filename);
	HttpMessage::setHeader(headers, "content-type", Response::getMimeType(request.getActualUri()));

    return (new Response(_status, headers, ""));
}
