#ifndef __EXCEPTDEF_H__
#define __EXCEPTDEF_H__

// 这里定义一点关于错误异常的宏

#include <stdexcept>
#include <cassert>

namespace mystl {
// assert，当expr为假，stderr打印错误信息，并且终止程序
#define MYSTL_DEBUG(expr) \
    assert(expr)

// what为字符串，抛出长度错误
#define THROW_LENGTH_ERROR_IF(expr, what) \
    if((expr)) throw std::length_error(what)

// 范围越界
#define THROW_OUT_OF_RANGE_IF(expr, what) \
  if ((expr)) throw std::out_of_range(what)

// 超时错误
#define THROW_RUNTIME_ERROR_IF(expr, what) \
  if ((expr)) throw std::runtime_error(what)

}

#endif