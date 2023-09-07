#include <iostream>
#include <string>
// template <typename T>

// class Result
// {
// private:
//     T data;
//     bool success;
// public:
//     Result(/* args */);
//     ~Result();
// };

// Result::Result(/* args */)
// {
// }

// Result::~Result()
// {
// }

// class Base {};

// class Derived : public Base {};

// std::string hoge(Base &b) { return ("base"); }
// std::string hoge(Derived &d) { return ("derived"); }

// std::string hoge(Base *b) { return ("base"); }
// std::string hoge(Derived *d) { return ("derived"); }

// int main()
// {
//     Base b;
//     Derived d;
//     std::cout << "b: " << hoge(b) << std::endl; // 5
//     std::cout << "d: " << hoge(d) << std::endl;  // 10

//     std::cout << "b: " << hoge(&b) << std::endl; // 5
//     std::cout << "d: " << hoge(&d) << std::endl; // 10

// 	Base &base_d = d;
// 	std::cout << "Base &base_d = d: " << hoge(base_d) << std::endl; // 5
//     std::cout << "((Base *)&d) : " << hoge((Base *)&d) << std::endl; // 5

//     std::cout << hoge(*((Base *)&d)) << std::endl; // 5

//     return (0);
// }

// int main()
// {
//     std::string str;
//     std::string::size_type size = str.max_size();
//     std::cout << size << std::endl;
//     str.reserve(size);
//     std::cout << str.capacity() << std::endl;
//     str.reserve(size + 1);
//     std::cout << str.capacity() << std::endl;
// }

// #include <stdlib.h>

// int main()
// {
//     // uint8_t *ptr = new uint8_t[0];
//     // if (ptr)
//     // {
//     //     std::cout << "ptr is not null" << std::endl;
//     //     // std::cout << "ptr: [" << std::to_string << "]" << std::endl;
//     //     printf("ptr: [%p]\n", ptr);
//     //     for (int i = 0; i < 3; i++)
//     //     {
//     //         std::cout << i <<  "[" << ptr[i] << "]" << std::endl;
//     //     }
//     // }
//     // else
//     //     std::cout << "ptr is null" << std::endl;

//     char *ptr;
//     ptr = static_cast<char *>(malloc(0));
//     if (ptr)
//     {
//         std::cout << "ptr is not null" << std::endl;
//         // std::cout << "ptr: [" << std::to_string << "]" << std::endl;
//         // printf("ptr: [%p]\n", ptr);
//         // for (int i = 0; i < 3; i++)
//         // {
//         //     std::cout << i <<  "[" << ptr[i] << "]" << std::endl;
//         // }
//     }
//     else
//         std::cout << "ptr is null" << std::endl;
// }


// #include <sstream>
// #include <iostream>
// #include <string>

// int main()
// {
//     {
//         std::stringstream ss("3a4ga");
//         unsigned int a;
//         ss >> std::hex >> a;
//         std::cout << a << std::endl;

//         std::string b;
//         ss >> b;
//         std::cout << b << std::endl;
//     }
//     std::cout << "---------" << std::endl;
//     {
//         std::stringstream ss("3a4ga");
//         std::string a;
//         ss >> std::hex >> a;
//         std::cout << a << std::endl;
//     }
// }

// /cgi/echo.php/aaa/bbb
// pathInfo: /aaa/bbb
// pathTranslated: /Users/ryoji/Desktop/42cursus/webserv/webserv/htdocs/cgi/echo.php/aaa/bbb
// scriptName: /echo.php
// directory /cgi

#include <string>
#include <iostream>

#include <string>
#include <iostream>

std::string makePathInfo(const std::string& str) {
    std::string::size_type pos = str.find(".php");
    if (pos == std::string::npos) {
        return ""; // ".php" が見つからない場合、空の文字列を返す
    }
    pos += 4; // ".php" の長さ分だけ位置を移動

    // pos 以降の文字列を返す
    if (pos < str.size()) {
        return str.substr(pos);
    } else {
        return "";
    }
}


std::string makeScriptName(const std::string& str) {
    std::string::size_type pos = str.rfind(".php");
    if (pos == std::string::npos) {
        return "";
    }
    std::string::size_type startPos = str.rfind('/', pos);
    if (startPos == std::string::npos) {
        return "";
    }
    return str.substr(startPos, pos + 4 - startPos);
}

std::string makeDirectory(const std::string& str) {
    std::string::size_type pos = str.rfind(".php");
    if (pos == std::string::npos) {
        return "";
    }
    std::string::size_type endPos = str.rfind('/', pos);
    if (endPos == std::string::npos) {
        return "";
    }
    std::string::size_type startPos = str.rfind('/', endPos - 1);
    if (startPos == std::string::npos) {
        return "";
    }
    return str.substr(startPos, endPos - startPos);
}

int main()
{
    std::string str = "/cgi/echo.php/aaa/bbb";
    // .php で終わる部分以降の部分を切り取る
    std::string pathInfo = makePathInfo(str);
    std::cout << "pathInfo: " << pathInfo << std::endl; // /aaa/bbb
    // .php のscriptName を取得する
    std::string scriptName = makeScriptName(str); // /echo.php
    std::cout << "scriptName: " << scriptName << std::endl;

    // 移動するべきディレクトリを取得する
    std::string directory = makeDirectory(str); // /cgi
    std::cout << "directory " << directory << std::endl;

    return 0;
}

