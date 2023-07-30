#ifndef REQUEST_HPP
#define REQUEST_HPP

#include <iostream>
#include <map>
#include <string>
#include "Socket.hpp"

class Request
{
public:
	// Constructors
	Request(const std::string &method, const std::string &uriAndQuery, const std::string &protocol,
			const std::map<std::string, std::string> &headers, const std::string &body);
	Request(const std::string &row_request);
	Request(const Request &other);

	// Destructor
	~Request();

	// Operators
	Request &operator=(const Request &rhs);

	// Getters/Setters
	std::string getMethod() const;
	std::string getUri() const;
	std::string	getProtocol() const;
	std::string getHeader(const std::string &key) const;
	std::string getBody() const;

	std::string	getActualUri() const;

	std::string	convertUritoPath(const std::string &uri);
	void		print_all() const;
	int			seturi();

	int	setaddr(ClSocket *clientSocket);
	static Request *parse(const std::string &row);

private:
	// メソッド名
	std::string _method;
	// URIとクエリ
	std::string _uriAndQuery;
	// URI
	std::string _uri;
	// クエリ
	std::string _query;
	// プロトコル
	std::string _protocol;
	// ヘッダ
	std::map<std::string, std::string> _headers;
	// ボディ
	std::string _body;
	// パス
	std::string _actual_uri;
	// IP
	std::string _ip;

	std::size_t _content_length;
	std::string	_content_type;
	std::string	_cgi_script_name;
	std::string	_path_info;
	std::string	_remote_addr;
	std::string	_remote_host;

	std::string	_server_name;
	std::string	_server_port;


	static	void		setRequestLine(const std::string &line);
	static	std::string	convertUritoPath(const std::string &uri, const std::string &path);
	static void			addHeaderToLower(std::map<std::string, std::string>& m, const std::string& inputStr, const std::string& keyword);
	static void			setRequestLine(const std::string &line, std::string &method, std::string &uri, std::string &protocol);
	// Not use
	Request();
};

#endif