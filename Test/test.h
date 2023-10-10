#ifndef __TEST_H__
#define __TEST_H__

#include <string>
#include <iostream>

namespace mystl {

// 遍历输出容器
#define COUT(container) do {                    \
    std::string con_name = #container;          \
    std::cout << con_name << " : ";             \
    for(auto it : container)                    \
        std::cout << it << " ";                 \
    std::cout<<std::endl;                       \
} while(0)                                      \


// 输出调用函数后的容器内元素的结果
#define FUN_AFTER(container, fun) do {              \
    std::string fun_name = #fun;                    \
    std::cout << "After " << fun_name << std::endl; \
    fun;                                            \
    COUT(container);                                \
} while(0)                                          \

// 输出容器函数的返回值
#define FUN_VALUE(fun) do {                         \
    std::string fun_name = #fun;                    \
    std::cout << fun_name << " : " << fun << "\n";  \
} while(0)                                          \


} // namespace mystl

#endif