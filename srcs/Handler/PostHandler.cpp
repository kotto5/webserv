#include "PostHandler.hpp"
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <filesystem>
#include "utils.hpp"

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

    std::ofstream ofs;
	std::string tmp = filename;
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
        std::cerr << "Error: can not create file in this name more" << std::endl;
		return (Response(500));
	}
	ofs.open(filedir + filename, std::ios::out);
    if (!ofs)
    {
        std::cerr << "Error: file not opened." << std::endl;
		return (Response(500));
    }
	std::string body = request.getBody();
    ofs << body;
    ofs.close();

	std::map<std::string, std::string> headers;
	pos = request.getUri().find_last_of('/');
    std::string uridir = request.getUri().substr(0, pos + 1);

	headers["Location"] = uridir + filename;
    return (Response(201, headers, ""));
}
