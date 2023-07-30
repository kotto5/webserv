#include "PostHandler.hpp"
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <filesystem>
#include "utils.hpp"
#include "Logger.hpp"

// Constructors
PostHandler::PostHandler()
{
	// 規定値を200に設定
	this->_status = 200;
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
 * @brief GETメソッドの処理を行う
 *
 * @param request リクエスト
 * @return Response レスポンス
 */

Response PostHandler::handleRequest(const Request &request)
{
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
		Logger::instance()->writeErrorLog(ErrorCode::POST_INDEX_FULL, NULL, &request);
		return (Response(500));
	}
	std::cout << filedir << std::endl;
	if (!pathExist(filedir.c_str()))
	{
		// ディレクトリが存在しない
		Logger::instance()->writeErrorLog(ErrorCode::POST_NOT_EXISTS, NULL, &request);
		return (Response(404));
	}
	std::ofstream ofs(filedir + filename, std::ios::out);
	if (!ofs)
	{
		if (errno == EACCES)
		{
			// ファイルにアクセスできない
			Logger::instance()->writeErrorLog(ErrorCode::POST_FILE_ACCESS, NULL, &request);
			return (Response(403));
		}
		Logger::instance()->writeErrorLog(ErrorCode::POST_FILE_OPEN, NULL, &request);
		return (Response(500));
    }
	std::string body = request.getBody();
    ofs << body;
    ofs.close();

	// レスポンスヘッダーの作成
	std::map<std::string, std::string> headers;
	pos = request.getUri().find_last_of('/');
    std::string uridir = request.getUri().substr(0, pos + 1);
	headers["Location"] = uridir + filename;
	headers["Content-Type"] = Response::getMimeType(request.getActualUri());

    return (Response(201, headers, ""));
}
