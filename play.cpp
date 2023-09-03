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

class Base {};

class Derived : public Base {};

std::string hoge(Base &b) { return ("base"); }
std::string hoge(Derived &d) { return ("derived"); }

std::string hoge(Base *b) { return ("base"); }
std::string hoge(Derived *d) { return ("derived"); }

int main()
{
    Base b;
    Derived d;
    std::cout << "b: " << hoge(b) << std::endl; // 5
    std::cout << "d: " << hoge(d) << std::endl;  // 10

    std::cout << "b: " << hoge(&b) << std::endl; // 5
    std::cout << "d: " << hoge(&d) << std::endl; // 10

	Base &base_d = d;
	std::cout << "Base &base_d = d: " << hoge(base_d) << std::endl; // 5
    std::cout << "((Base *)&d) : " << hoge((Base *)&d) << std::endl; // 5

    std::cout << hoge(*((Base *)&d)) << std::endl; // 5

    return (0);
}