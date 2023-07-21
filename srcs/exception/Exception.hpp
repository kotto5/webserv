#ifndef EXCEPTION_HPP
#define EXCEPTION_HPP

#include <stdexcept>

class Exception : public std::runtime_error
{
	public:
		Exception(const std::string& message);
		~Exception();
};

#endif

//std::runtime_errorクラスのコンストラクタはエラーメッセージの文字列を引数として受け取り、それを内部で保存します。
//what()メソッドはその保存されたエラーメッセージを返します。