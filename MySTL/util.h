#ifndef __UTIL_H__
#define __UTIL_H__

// 这个文件定义了一些通用的工具，包括move, forward, swap等函数，以及pair结构体

#include <cstddef>
#include <type_traits>
#include <istream>

namespace mystl {

// move 将左值变成右值
/* 
template<typename _Tp>
struct remove_reference
{ typedef _Tp   type; };

template<typename _Tp>
struct remove_reference<_Tp&>
{ typedef _Tp   type; };
template<typename _Tp>
struct remove_reference<_Tp&&>
{ typedef _Tp   type; }; 
*/
// type为最原始的类型，萃取出type，然后类型转化为右值
template <class T>
typename std::remove_reference<T>::type&& move(T&& arg) {
    return static_cast<typename std::remove_reference<T>::type&&>(arg); //返回arg的右值，传入的参数可能为左值引用和右值引用
}

// forward，左值引用返回左值，右值引用返回右值
/* 
T& & -> T& （对左值引用的左值引用是左值引用）
T& && -> T& （对左值引用的右值引用是左值引用）
T&& & ->T& （对右值引用的左值引用是左值引用）
T&& && ->T&& （对右值引用的右值引用是右值引用） 
*/
template <class T>
T&& forward(typename std::remove_reference<T>::type& arg) noexcept
{
  return static_cast<T&&>(arg);
}

template <class T>
T&& forward(typename std::remove_reference<T>::type&& arg) noexcept
{
  static_assert(!std::is_lvalue_reference<T>::value, "bad forward");
  return static_cast<T&&>(arg);
}

// swap
template <class Tp>
void swap(Tp& lhs, Tp& rhs) {
    Tp tmp(mystl::move(lhs));
    lhs = mystl::move(rhs);
    rhs = mystl::move(tmp);
}

// pair
template <class T1, class T2>
struct pair{
    typedef T1 first_type;
    typedef T2 second_type;

    first_type first;
    second_type second;

    pair() : first(), second() {}
    pair(const T1& a, const T2& b) : first(a), second(b) {}
    pair(T1&& a, T2&& b) : first(mystl::forward<T1>(a)), second(mystl::forward<T2>(b)) {}

    pair(const pair& rhs) : first(rhs.first), second(rhs.second) {}
    pair(pair&& rhs) : first(mystl::forward<T1>(rhs.first)), second(mystl::forward<T2>(rhs.second)) {}

    pair& operator=(const pair& rhs) {
      if(this != &rhs) {
        first = rhs.first;
        second = rhs.second;
      }
      return *this;
    }

    pair& operator=(pair&& rhs)
    {
      if (this != &rhs)
      {
        first = mystl::move(rhs.first);
        second = mystl::move(rhs.second);
      }
      return *this;
    }

    template <class Other1, class Other2>
    pair& operator=(pair<Other1, Other2>&& other)
    {
      first = mystl::forward<Other1>(other.first);
      second = mystl::forward<Other2>(other.second);
      return *this;
    }


    ~pair() = default;

    // 定义输出流重载
    template <class U1, class U2>
    friend std::ostream& operator<<(std::ostream& os, const pair<U1, U2>& rhs);
};

template <class U1, class U2>
std::ostream& operator<<(std::ostream& os, const pair<U1, U2>& rhs) {
    os << "(" << rhs.first << ", " << rhs.second << ")";
    return os;
}

template <class T1, class T2>
bool operator==(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
  return lhs.first == rhs.first && lhs.second && rhs.second;
}

template <class T1, class T2>
bool operator!=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
  return !(lhs == rhs);
}

template <class T1, class T2>
bool operator<(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
  return lhs.first < rhs.first ||  (lhs.first == rhs.first && lhs.second < rhs.second);
}

template <class T1, class T2>
bool operator>=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
  return !(lhs < rhs);
}

template <class T1, class T2>
bool operator>(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
  return rhs < lhs;
}

template <class T1, class T2>
bool operator<=(const pair<T1, T2>& lhs, const pair<T1, T2>& rhs) {
  return !(lhs > rhs);
}


}

#endif