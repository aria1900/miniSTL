#ifndef __STACK_H__
#define __STACK_H__

#include "deque.h"

namespace mystl{

// 栈的定义，默认底层容器为deque
template <class T, class Container = mystl::deque<T>>
class stack {
public:
    typedef Container container_type;

    typedef typename Container::value_type value_type;
    typedef typename Container::size_type size_type;
    typedef typename Container::reference reference;
    typedef typename Container::const_reference const_reference;

private:
    container_type con_;

public:
    // 构造、赋值、移动函数
    stack() = default;

    stack(const stack& other) : con_(other.con_) {}

    stack(stack&& rhs) : con_(rhs.con_) {}

    template <class Iterator>
    stack(Iterator first, Iterator last) : con_(first, last) {}

    stack(std::initializer_list<T> ilist) : con_(ilist) {}

    stack& operator=(const stack& rhs) {
        con_ = rhs.con_;
        return *this;
    }

    stack& operator=(stack&& rhs) {
        con_ = mystl::move(rhs.con_);
        return *this;
    }

    stack& operator=(std::initializer_list<T> ilist) {
        con_ = ilist;
        return *this;
    }

    ~stack() = default;     // deque自己会析构

    // 元素访问
    reference top() { return con_.back(); }
    const_reference top() const { return con_.back(); }

    // 容量
    bool empty() const { return con_.empty(); }
    size_type size() const { return con_.size(); }

    // push / pop
    void push(const value_type& value) {
        con_.push_back(value);
    }

    void pop() {
        con_.pop_back();
    }

    void clear() {
        while(!empty()) {
            pop();
        }
    }

    void swap(stack& rhs) {
        mystl::swap(con_, rhs.con_);
    }

public:
    bool operator==(const stack& rhs) { return con_ == rhs.con_; }
    bool operator<(const stack& rhs) { return con_ < rhs.con_; }
};

// 重载比较符运算函数
template <class T, class Container>
bool operator==(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{
  return lhs == rhs;
}

template <class T, class Container>
bool operator!=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{
  return !(lhs == rhs);
}

template <class T, class Container>
bool operator<(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{
  return lhs < rhs;
}

template <class T, class Container>
bool operator>=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{
  return !(lhs < rhs);
}

template <class T, class Container>
bool operator>(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{
  return rhs < lhs;
}

template <class T, class Container>
bool operator<=(const stack<T, Container>& lhs, const stack<T, Container>& rhs)
{
  return !(lhs > rhs);
}

}

#endif