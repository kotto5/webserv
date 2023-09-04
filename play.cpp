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

#include <stdlib.h>

int main()
{
    // uint8_t *ptr = new uint8_t[0];
    // if (ptr)
    // {
    //     std::cout << "ptr is not null" << std::endl;
    //     // std::cout << "ptr: [" << std::to_string << "]" << std::endl;
    //     printf("ptr: [%p]\n", ptr);
    //     for (int i = 0; i < 3; i++)
    //     {
    //         std::cout << i <<  "[" << ptr[i] << "]" << std::endl;
    //     }
    // }
    // else
    //     std::cout << "ptr is null" << std::endl;

    char *ptr;
    ptr = static_cast<char *>(malloc(0));
    if (ptr)
    {
        std::cout << "ptr is not null" << std::endl;
        // std::cout << "ptr: [" << std::to_string << "]" << std::endl;
        // printf("ptr: [%p]\n", ptr);
        // for (int i = 0; i < 3; i++)
        // {
        //     std::cout << i <<  "[" << ptr[i] << "]" << std::endl;
        // }
    }
    else
        std::cout << "ptr is null" << std::endl;
}

