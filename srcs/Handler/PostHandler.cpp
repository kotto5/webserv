#include "PostHandler.hpp"
#include <fstream>
#include <sstream>
#include <unistd.h>

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
    // create file with request.uri member
    std::string path = request.getUri();
    std::string body = request.getBody();

    std::ofstream ofs;
	ofs.open(request.getActualUri(), std::ios::out);
    if (!ofs)
    {
        std::cerr << "Error: file not opened." << std::endl;
		return (Response(500));
    }
    ofs << body;
    ofs.close();
    return (Response(201));
}
