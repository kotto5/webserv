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
	this->_status = 200;
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
 * @brief GETメソッドの処理を行う
 *
 * @param request リクエスト
 * @return Response レスポンス
 */

Response DeleteHandler::handleRequest(const Request &request)
{
    // create file with request.uri member
    std::string filename = request.getActualUri();
    std::string body = request.getBody();

    std::ofstream ofs;
	std::string tmp = filename;
	for (std::size_t i = 0; i != SIZE_MAX; ++i)
	{
		if (!pathExist(tmp.c_str()))
		{
			filename = tmp;
			break;
		}
		tmp = filename + std::to_string(i);
	}
	if (pathExist(filename.c_str()))
	{
        std::cerr << "Error: can not create file in this name more" << std::endl;
		return (Response(500));
	}
	ofs.open(filename, std::ios::out);
    if (!ofs)
    {
        std::cerr << "Error: file not opened." << std::endl;
		return (Response(500));
    }
    ofs << body;
    ofs.close();
	std::map<std::string, std::string> headers;
	headers["Location"] = filename;
    return (Response(201, headers, ""));
}
