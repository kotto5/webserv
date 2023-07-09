#include "GetHandler.hpp"
#include <fstream>
#include <sstream>

// Constructors
GetHandler::GetHandler()
{
	// 規定値を200に設定
	this->_status = 200;
}

GetHandler::GetHandler(const GetHandler &other)
{
	this->_status = other._status;
}

// Destructor
GetHandler::~GetHandler() {}

// Operators
GetHandler &GetHandler::operator=(const GetHandler &rhs)
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

#include <unistd.h>

Response GetHandler::handleRequest(const Request &request)
{
	int	pipe_fd[2];
	if (pipe(pipe_fd))
	{
		std::cout << "pipe error" << std::endl;
		exit(1);
	}
	pid_t pid = fork();
	if (pid == 0)
	{
		close(pipe_fd[0]);
		dup2(pipe_fd[1], 1);
		close(pipe_fd[1]);
		std::string path = "/Users/kakiba/AAproject/42_webserv/docs/index.php";
		// std::string query = request.getQuery();
		// std::string path_query = path + "?" + query;
		std::string path_query = path;
		char	*php_path = (char *)"/usr/bin/php";
		char *argv[] = {php_path, const_cast<char *>(path_query.c_str()), NULL};
		char *envp[] = {NULL};
		execve(php_path, argv, envp);
		perror("execve");
		exit(0);
	}
	else if (pid < 0)
	{
		std::cout << "fork error" << std::endl;
		exit(1);
	}
	else
	{
		close(pipe_fd[1]);
		waitpid(pid, NULL, 0);
		std::stringstream buffer;
		char buf[1024];
		int len;
		while ((len = read(pipe_fd[0], buf, 1024)) > 0)
			buffer.write(buf, len);
		close(pipe_fd[0]);
		std::string body = buffer.str();
		std::map<std::string, std::string> headers;
		headers["Content-Type"] = "text/html; charset=UTF-8";
		Response res(_status, headers, body);
		return res;
	}

	// URIからファイルを開く
	std::ifstream htmlFile(request.getUri());
	if (!htmlFile.is_open())
	{
		// ファイルが開けなかった場合は404を返す
		this->_status = 404;
	}

	// ファイルの内容を読み込む
	std::stringstream buffer;
	buffer << htmlFile.rdbuf();
	htmlFile.close();

	// レスポンスを作成して返す
	std::map<std::string, std::string> headers;
	headers["Content-Type"] = Response::getMimeType(request.getUri());
	Response res(_status, headers, buffer.str());

	return res;
}
