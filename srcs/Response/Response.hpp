#ifndef RESPONSE_HPP
#define RESPONSE_HPP

#include <iostream>
#include <map>
#include <string>

class Response
{
public:
	// Constructors
	Response(const int status, std::map<std::string, std::string> headers, const std::string &body);
	Response(const int status);
	Response(const Response &other);

	// Destructor
	~Response();

	// Operators
	Response &operator=(const Response &rhs);

	// Member functions
	std::string toString() const;
	static std::string getMimeType(const std::string &filename);

	// Getters/setters
	int getStatus() const;
	std::string getHeader(const std::string &key) const;
	std::string getBody() const;

private:
	// ステータスコード
	int _status;
	// ヘッダー
	std::map<std::string, std::string> _headers;
	// ボディ
	std::string _body;

	// 拡張子とMIMEタイプの対応表
	static std::map<std::string, std::string> extensionToMime;

	// ステータスコードから該当するメッセージを取得する
	std::string getStatusMessage(int statusCode) const;

	// 現在日時をセットする
	void setDate();

	// Content-Lengthをセットする
	void setContentLength();

	// Serverをセットする
	void setServer();

	// Not use
	Response();
};

#endif