#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "deque.h"
#include "vector.h"
#include "functional.h"
#include "heap_algo.h"

namespace mystl {

template <class T, class Container = mystl::deque<T>>
class queue{
public:
    typedef Container container_type;

    typedef typename Container::value_type value_type;
    typedef typename Container::size_type size_type;
    typedef typename Container::reference reference;
    typedef typename Container::const_reference const_reference;

private:
    container_type con_;

public:
    // 构造、移动、赋值函数
    queue() = default;

    queue(const queue& other) : con_(other.con_) {}

    queue(queue&& rhs) : con_(rhs.con_) {}

    template <class Iterator>
    queue(Iterator first, Iterator last) : con_(first, last) {}

    queue(std::initializer_list<T> ilist) : con_(ilist) {}

    queue& operator=(const queue& rhs) {
        con_ = rhs.con_;
        return *this;
    }

    queue& operator=(queue&& rhs) {
        con_ = mystl::move(rhs.con_);
        return *this;
    }

    queue& operator=(std::initializer_list<T> ilist) {
        con_ = ilist;
        return *this;
    }

    ~queue() = default;

    // 元素访问
    reference front() { return con_.front(); }
    const_reference front() const { return con_.front(); }

    // 容量
    bool empty() const { return con_.empty(); }
    size_type size() const { return con_.size(); }

    // push / pop
    void push(const value_type& value) {
        con_.push_back(value);    // 队尾进
    }

    void pop() {
        con_.pop_front();   // 队头出
    }

    void clear() {
        while(!empty()) {
            pop();
        }
    }

    void swap(queue& rhs) {
        mystl::swap(con_, rhs.con_);
    }

public:
    bool operator==(const queue& rhs) { return con_ == rhs.con_; }
    bool operator<(const queue& rhs) { return con_ < rhs.con_; }
};

template <class T, class Container>
bool operator==(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
  return lhs == rhs;
}

template <class T, class Container>
bool operator!=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
  return !(lhs == rhs);
}

template <class T, class Container>
bool operator<(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
  return lhs < rhs;
}

template <class T, class Container>
bool operator>=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
  return !(lhs < rhs);
}

template <class T, class Container>
bool operator>(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
  return rhs < lhs;
}

template <class T, class Container>
bool operator<=(const queue<T, Container>& lhs, const queue<T, Container>& rhs)
{
  return !(lhs > rhs);
}


// priority_queue
template <class T, class Container = mystl::vector<T>,
          class Compare = mystl::less<typename Container::value_type>>
class priority_queue {
public:

    typedef Container container_type;
    typedef Compare value_compare;

    typedef typename Container::value_type value_type;
    typedef typename Container::size_type size_type;
    typedef typename Container::reference reference;
    typedef typename Container::const_reference const_reference;

private:
    container_type con_;    // 底层容器
    value_compare comp_;    // 比较规则 operator<

public:
    priority_queue() = default;

    priority_queue(const Compare& c) : con_(), comp_(c) {}

    template <class Iterator>
    priority_queue(Iterator first, Iterator last) : con_(first, last) {
        mystl::make_heap(con_.begin(), con_.end(), comp_);
    }

    priority_queue(std::initializer_list<T> ilist) : con_(ilist) {
        mystl::make_heap(con_.begin(), con_.end(), comp_);
    }

    priority_queue(const Container& rhs) : con_(rhs) {
        mystl::make_heap(con_.begin(), con_.end(), comp_);
    }

    priority_queue(Container&& rhs) : con_(mystl::move(rhs)) {
        mystl::make_heap(con_.begin(), con_.end(), comp_);
    }

    priority_queue(const priority_queue& rhs) : con_(rhs.con_), comp_(rhs.comp_) {
        mystl::make_heap(con_.begin(), con_.end(), comp_);
    }

    priority_queue(priority_queue&& rhs) : con_(mystl::move(rhs.con_)), comp_(rhs.comp_) {
        mystl::make_heap(con_.begin(), con_.end(), comp_);
    }

    // operator =
    priority_queue& operator=(const priority_queue& rhs) {
        con_ = rhs.con_;
        comp_ = rhs.comp_;
        mystl::make_heap(con_.begin(), con_.end(), comp_);
        return *this;
    }

    priority_queue& operator=(priority_queue&& rhs) {
        con_ = mystl::move(rhs.con_);
        comp_ = rhs.comp_;
        mystl::make_heap(con_.begin(), con_.end(), comp_);
        return *this;
    }

    ~priority_queue() = default;

public:
    // 访问元素相关
    /* const_ */reference top()/*  const */ { return con_.front(); }

    // 容量相关
    bool empty() const { return con_.empty(); }
    size_type size() const { return con_.size(); }

    // 修改容器相关操作
    void push(const value_type& value) {
        // 先容器入元素，再push_heap调整堆
        con_.push_back(value);
        mystl::push_heap(con_.begin(), con_.end(), comp_);
    }

    void pop() {
        mystl::pop_heap(con_.begin(), con_.end(), comp_);
        con_.pop_back();
    }

    void clear() {
        while(!empty()) {
            pop();
        }
    }

public:
  bool operator==(const priority_queue& rhs) {
    return con_ == rhs.con_;
  }
  bool operator!=(const priority_queue& rhs) {
    return con_ != rhs.con_;
  }
};

// 重载比较操作符，全局
template <class T, class Container, class Compare>
bool operator==(const priority_queue<T, Container, Compare>& lhs,
                const priority_queue<T, Container, Compare>& rhs)
{
   return lhs == rhs;
}

template <class T, class Container, class Compare>
bool operator!=(const priority_queue<T, Container, Compare>& lhs,
                const priority_queue<T, Container, Compare>& rhs)
{
  return lhs != rhs;
}

}



#endif