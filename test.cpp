#include <vector>
#include <iostream>
#include <unistd.h>

// int main()
// {
//     std::vector<int *> *v = new std::vector<int *>;

//     int *a = new int(1);
//     v->push_back(a);
//     int *b = new int(2);
//     v->push_back(b);

//     std::vector<int *>::iterator itr = v->begin();
//     for (; itr != v->end(); itr++)
//     {
//         std::cout << **itr << std::endl;
//         *itr = NULL;
//     }
//     a = NULL;
//     b = NULL;
//     system("leaks -q a.out");
//     return 0;
// }

// int main()
// {
//     std::vector<int *> *v = new std::vector<int *>;

//     int *a = new int(1);
//     v->push_back(a);
//     int *b = new int(2);
//     v->push_back(b);
//     a = NULL;
//     b = NULL;

//     std::vector<int *>::iterator itr = v->begin();
//     for (; itr != v->end(); itr++)
//     {
//         std::cout << **itr << std::endl;
//         // *itr = NULL;
//     }
//     v->clear();
//     v = NULL;
//     system("leaks -q a.out");
//     return 0;
// }

// int main()
// {
//     std::vector<int &> *v = new std::vector<int &>;

//     int *a = new int(1);
//     v->push_back(*a);
//     int *b = new int(2);
//     v->push_back(*b);
//     a = NULL;
//     b = NULL;

//     std::vector<int &>::iterator itr = v->begin();
//     for (; itr != v->end(); itr++)
//     {
//         std::cout << *itr << std::endl;
//         // *itr = NULL;
//     }
//     v->clear();
//     v = NULL;
//     system("leaks -q a.out");
//     return 0;
// }

// int main()
// {
//     std::vector<const int &> *v = new std::vector<const int &>;

//     int *a = new int(1);
//     v->push_back(*a);
//     int *b = new int(2);
//     v->push_back(*b);
//     a = NULL;
//     b = NULL;

//     std::vector<const int &>::iterator itr = v->begin();
//     for (; itr != v->end(); itr++)
//     {
//         std::cout << *itr << std::endl;
//         // *itr = NULL;
//     }
//     v->clear();
//     v = NULL;
//     system("leaks -q a.out");
//     return 0;
// }
