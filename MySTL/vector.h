#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <initializer_list>
#include <type_traits>

#include "iterator.h"
#include "exceptdef.h"
#include "util.h"
#include "allocator.h"
#include "construct.h"
#include "uninitialized.h"
#include "algobase.h"

namespace mystl {

// T是vector的存储类型，不接受allocator模板参数
template <class T>
class vector {
public:
    // vector的内置型别
    typedef mystl::allocator<T> allocator_type;
    typedef mystl::allocator<T> data_allocator;

    allocator_type get_allocator() const { return allocator_type(); }

    typedef T value_type;
    typedef value_type* pointer;
    typedef const value_type* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    // vector的迭代器是裸指针
    typedef value_type* iterator;
    typedef const value_type* const_iterator;
    typedef mystl::reverse_iterator<iterator> reverse_iterator;
    typedef mystl::reverse_iterator<const_iterator> const_reverse_iterator;

private:
    iterator start_;            // 使用空间的头部
    iterator finish_;           // 使用空间的尾部
    iterator end_of_storage_;    // 可用空间的尾部

public:
    // 构造、复制、赋值、移动、析构函数

    // 默认构造函数分配16个capacity
    vector() {
       start_ = data_allocator::allocate(16);
       finish_ = start_;
       end_of_storage_ = start_ + 16;
    }

    explicit vector(size_type n) {
        fill_init(n, value_type());
    }

    vector(size_type n, const value_type& value) {
        fill_init(n, value);
    }

    // 要保证这个是迭代器的分支，可以采用分支的方法
    template <class Iterator>
    vector(Iterator first, Iterator last) {
        typedef typename is_integral<Iterator>::value is_Int;
        range_ctor_aux(first, last, is_Int());
    }

    // copy_ctor
    vector(const vector& rhs) {
        init_space(rhs.size(), rhs.capacity());
        uninitialized_copy(rhs.begin(), rhs.end(), start_);
    }

    // 移动构造函数，相当于把rhs占为己有
    vector(vector&& rhs) 
        :start_(rhs.start_), 
        finish_(rhs.finish_), 
        end_of_storage_(rhs.end_of_storage_) {
        rhs.start_ = nullptr;
        rhs.finish_ = nullptr;
        rhs.end_of_storage_ = nullptr;
    }

    // ininitializer_list
    vector(std::initializer_list<value_type> ilist) {
        range_init(ilist.begin(), ilist.end());
    }

    // 其实这里capacity也没有严格统一
    vector& operator=(const vector& rhs) {
       if(&rhs != this) {
            const size_type rsize = rhs.size();
            if(rsize > capacity()) {
                // 要扩容了
                iterator tmp = allocate_and_copy(rsize, rhs.begin(), rhs.end());
                mystl::destroy(start_, finish_);
                data_allocator::deallocate(start_);
                start_ = tmp;
                end_of_storage_ = start_ + rsize;
            } else if(size() > rsize) {
                // 要析构一点了
                iterator i = mystl::copy(rhs.begin(), rhs.end(), start_);
                mystl::destroy(i, finish_);
            } else {
                // size() <= rsize <= capacity
                // 不存在扩容和析构问题，直接copy
                mystl::copy(rhs.begin(), rhs.begin() + size(), start_);
                uninitialized_copy(rhs.begin() + size(), rhs.end(), finish_);
            }
            finish_ = start_ + rsize;
       } 
       return *this;
    }

    // 移动赋值,比起移动构造多了个析构释放原有空间
    vector& operator=(vector&& rhs) {
        destroy_and_deallocate(start_, finish_, capacity());
        start_ = rhs.start_;
        finish_ = rhs.finish_;
        end_of_storage_ = rhs.end_of_storage_;
        rhs.start_ = nullptr;
        rhs.finish_ = nullptr;
        rhs.end_of_storage_ = nullptr;
        return *this;
    }

    // 构造再交换
    vector& operator=(std::initializer_list<value_type> ilist) {
        vector tmp(ilist);
        swap(tmp);
        return *this;
    }

    ~vector() {
        destroy_and_deallocate(start_, finish_, capacity());
        start_ = nullptr;
        finish_ = nullptr;
        end_of_storage_ = nullptr;
    }
    
public:
    // 迭代器的相关操作
    
    iterator begin() { return start_; }
    const_iterator begin() const { return start_; }
    iterator end() { return finish_; }
    const_iterator end() const { return finish_; }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }
    const_reverse_iterator crbegin() const { return rbegin(); }
    const_reverse_iterator crend() const { return rend(); }

    // 容量相关的操作

    bool empty() const { return start_ == finish_; }
    size_type size() const { return (size_type)(finish_ - start_); }
    size_type capacity() const { return (size_type)(end_of_storage_ - start_); }
    size_t max_size() const { return size_type(-1) / sizeof(T); }
    void reserve(size_type n); //保留n个内存
    void shrink_to_fit(); //将多余的内存不要了

    // 访问元素相关操作

    reference operator[] (size_type n) {
        THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::operator[] out of range.");
        return *(start_ + n);
    }

    const_reference operator[](size_type n) const {
        THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::operator[] out of range.");
        return *(start_ + n);
    }

    reference at(size_type n) {
        THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() out of range.");
        return (*this)[n];
    }

    const_reference at(size_type n) const {
        THROW_OUT_OF_RANGE_IF(!(n < size()), "vector<T>::at() out of range.");
        return (*this)[n];
    }

    reference front() {
        MYSTL_DEBUG(!empty()); //非空
        return *start_;
    }

    const_reference front() const {
        MYSTL_DEBUG(!empty()); 
        return *start_;
    }

    reference back() {
        MYSTL_DEBUG(!empty());
        return *(finish_ - 1);
    }

    const_reference back() const {
        MYSTL_DEBUG(!empty());
        return *(finish_ - 1);
    }

    pointer data() { return start_; } // 返回内存地址
    const_pointer data() const { return start_; }

    // 修改容器相关操作

    // assign

    void assign(size_type n, const value_type& value) {
        fill_assign(n, value);
    }

    // 可能产生分支
    template <class Iterator>
    void assign(Iterator first, Iterator last) {
        typedef typename is_integral<Iterator>::value is_Int;
        copy_assign(first, last, is_Int());
    }

    void assign(std::initializer_list<T> ilist) {
        copy_assign(ilist.begin(), ilist.end(), false_type());
    }

    // push_back / pop_back
    void push_back();
    void push_back(const value_type& value);

    void pop_back();

    // insert
    iterator insert(iterator pos, const value_type& value);
    void insert(iterator pos, size_type n, const value_type& value);
    // 与上面的版本需要区分
    template <class Iterator>
    void insert(iterator pos, Iterator first, Iterator last);


    // erase
    iterator erase(iterator pos);
    iterator erase(iterator first, iterator last);

    // clear
    void clear() { erase(begin(), end()); }

    // resize 如果new_size大，那么就填充value;如果new_size小，就把多余的析构了
    void resize(size_type new_size);
    void resize(size_type new_size, const value_type& value); 

    // swap

    void swap(vector & rhs) {
        if(&rhs != this) {
            mystl::swap(rhs.start_, start_);
            mystl::swap(rhs.finish_, finish_);
            mystl::swap(rhs.end_of_storage_, end_of_storage_);
        }    
    }

private:
    // 内部辅助函数

    // 申请cap个空间，并且以size为大小
    void init_space(size_type size, size_type capacity) {
        try{
            start_ = data_allocator::allocate(capacity);
            finish_ = start_ + size;
            end_of_storage_ = start_ + capacity;
        } catch(...) {
            start_ = nullptr;
            finish_ = nullptr;
            end_of_storage_ = nullptr;
        }
    }

    // 分配n个空间，并以value初始化
    void fill_init(size_type n, const value_type& value) {
        const size_type init_size = mystl::max(static_cast<size_type>(16), n);
        init_space(n, init_size); //调整过了指针
        mystl::uninitialized_fill_n(start_, n, value); // 构造默认对象
    }

    // 这个就是(size, value)的构造函数的分支走到这里来了
    template <class Integer>
    void range_ctor_aux(Integer n, Integer value, true_type) {
        fill_init(n, value);
    }

    // 迭代器分支
    template <class Iterator>
    void range_ctor_aux(Iterator first, Iterator last, false_type) {
        MYSTL_DEBUG((first <= last)); //保证区间正确
        range_init(first, last);
    }

    // 以[first, last) 来初始化vector
    template <class Iterator>
    void range_init(Iterator first, Iterator last) {
        const size_type size = mystl::distance(first, last);
        const size_type init_size = mystl::max(static_cast<size_type>(16), size);
        init_space(size, init_size);
        mystl::uninitialized_copy(first, last, start_);
    }

    // 分配n个空间，并且把[first, last) 拷贝
    iterator allocate_and_copy(size_type n, const_iterator first, const_iterator last) {
        iterator result = data_allocator::allocate(n);
        try{
            uninitialized_copy(first, last, result);
            return result;
        } catch(...) {
            data_allocator::deallocate(result, n);
        }   
        return result;
    }

    // 析构[first, last) 释放n个内存
    void destroy_and_deallocate(iterator first, iterator last, size_type n) {
        mystl::destroy(first, last);
        data_allocator::deallocate(first, n);
    }

    // 该函数为assign的辅助函数，填充n个value
    void fill_assign(size_type n, const T& value) {
        if(n > capacity()) {
            vector tmp(n, value); //调用构造函数，类内部不用T
            swap(tmp);
        }else if(n > size()) {
            // 需要填充
            mystl::fill(start_, finish_, value);
            finish_ = mystl::uninitialized_fill_n(finish_, n - size(), value);
        }else {
            // 要析构一些
            iterator i = mystl::fill_n(start_, n, value);
            erase(i, finish_); //erase会调整finish_
        }
    }

    // 将区间内的赋值到本对象
    // 就是size value 分支
    template <class Integer>
    void copy_assign(Integer n, Integer value, true_type) {
        fill_assign(n, value);
    }
    // 真正的迭代器分支
    template <class Iterator>
    void copy_assign(Iterator first, Iterator last, false_type) {
        const auto n = mystl::distance(first, last);
        if(n > capacity()) {
            vector tmp(first, last);
            swap(tmp);
        }else if(n > size()) {
            iterator i = first;
            mystl::advance(i, size()); //前进size个位置
            mystl::copy(first, i, start_);
            finish_ = mystl::uninitialized_copy(i, last, finish_);
        }else {
            // 析构
            iterator i = mystl::copy(first, last, start_);
            erase(i, finish_);
        }
    }


    // 插入的辅助函数，在指定位置插入一元素，扩容逻辑也在这里
    void insert_aux(iterator pos) {
        insert_aux(pos, value_type());
    }

    void insert_aux(iterator pos, const value_type& value) {
        if(finish_ != end_of_storage_) {
            // 还有空余的空间
            mystl::construct(finish_, *(finish_ - 1)); //把最后一个位置的构造在新的位置
            ++finish_;
            value_type value_copy = value; // 防止移动赋值，把原先的资源释放了
            std::copy_backward(pos, finish_ - 2, finish_ - 1);
            *pos = value_copy;
        } else{
            // 内存不够
            const size_type old_size = size();
            const size_type len = old_size != 0 ? old_size * 2 : 16; 
            iterator new_start = data_allocator::allocate(len);
            iterator new_finish = new_start;
            try {
                // 分为3步，第一段copy，然后插入的值构造，然后第二段copy
                new_finish = mystl::uninitialized_copy(start_, pos, new_start);
                // 构造插入的值
                mystl::construct(new_finish, value);
                ++new_finish;
                new_finish = mystl::uninitialized_copy(pos, finish_, new_finish);
            } catch(...) {
                // 否则析构新的，然后把新的内存释放
                mystl::destroy(new_start, new_finish);
                data_allocator::deallocate(new_finish, len);
                throw;
            }
            mystl::destroy(start_, finish_);
            data_allocator::deallocate(start_, old_size);
            start_ = new_start;
            finish_ = new_finish;
            end_of_storage_ = new_start + len;
        }
    }

    // pos处插入n个value
    // 这里分情况是因为，如果情况2的话，直接copy_backward，然后fill
    // 会有未初始化的内存在中间，需要特殊调用uninit函数
    //        pos       n == 2   size = 6
    //  1 2 3 4   5  6  0  0    n比插入点之后的元素少

    //        pos       n == 3   size = 5
    //  1 2 3 4   5  0  0  0    n比插入点之后的元素多(等于)
    void fill_insert(iterator pos, size_type n, const value_type& value) {
        if(n == 0) return;

        if(n <= (size_type)(end_of_storage_ - finish_)) {
            // 备用空间足够
            const value_type value_copy = value;
            const size_type elems_afer_pos = finish_ - pos;
            iterator old_finish = finish_;
            if(n < elems_afer_pos) {
                // n比插入点之后的元素少
                mystl::uninitialized_copy(finish_ - n, finish_, finish_);
                finish_ += n;
                mystl::copy_backward(pos, old_finish - n, old_finish);
                mystl::fill_n(pos, n, value_copy);
            } else {
                // n比插入点之后的元素等于或多
                finish_ = mystl::uninitialized_fill_n(finish_, n - elems_afer_pos, value_copy);
                finish_ = mystl::uninitialized_copy(pos, old_finish, finish_);
                mystl::fill(pos, old_finish, value_copy);
            }
        } else {
            // 空间不够得开新内存了, 和上面insert_aux的一样
            size_type old_size = size();
            size_type len = old_size + mystl::max(old_size, n);
            iterator new_start = data_allocator::allocate(len);
            iterator new_finish = new_start;
            try {
                new_finish = mystl::uninitialized_copy(start_, pos, new_finish);
                new_finish = mystl::uninitialized_fill_n(pos, n, value);
                new_finish = mystl::uninitialized_copy(pos, finish_, new_finish);
            } catch(...) {
                mystl::destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, len);
                throw;
            }
            mystl::destroy(start_, finish_);
            data_allocator::deallocate(start_, old_size);
            start_ = new_start;
            finish_ = new_finish;
            end_of_storage_ = new_start + len;
        }
    }

    template <class Integer>
    void insert_range(iterator pos, Integer n, Integer value, true_type) {
        fill_insert(pos, n, value);
    }

    template <class Iterator>
    void insert_range(iterator pos, Iterator first, Iterator last, false_type) {
        MYSTL_DEBUG(pos >= begin() && pos <= end() && first <= last);
        if(first == last) return;

        auto n = (size_type)mystl::distance(first, last); //返回的是Iterator中的difference_type
        if(n <= (end_of_storage_ - finish_)) {
            // 空间足够
            const auto elems_after_pos = finish_ - pos;
            iterator old_finish = finish_;
            if(n < elems_after_pos) {
                // 新增的个数小于pos之后的
                mystl::uninitialized_copy(finish_ - n, finish_, finish_);
                finish_ += n;
                mystl::copy_backward(pos, old_finish - n, old_finish);
                mystl::copy(first, last, pos);
            }else{
                // 新增的个数大于等于pos之后的个数
                Iterator mid = first;
                mystl::advance(mid, elems_after_pos);
                finish_ = uninitialized_copy(mid, last, finish_); // 把右半部分先填在未初始化的内存里
                finish_ = uninitialized_copy(pos, old_finish, finish_);
                mystl::uninitialized_copy(first, mid, pos);
            }
        }else {
            // 空间不够，重新分配空间，然后3段copy即可
            const size_type old_size = size();
            const size_type len = old_size + mystl::max(n, old_size);
            iterator new_start = data_allocator::allocate(len);
            iterator new_finish = new_start;
            try{
                new_finish = mystl::uninitialized_copy(start_, pos, new_finish);
                new_finish = mystl::uninitialized_copy(first, last, new_finish);
                new_finish = mystl::uninitialized_copy(pos, finish_, new_finish);
            } catch(...) {
                mystl::destroy(new_start, new_finish);
                data_allocator::deallocate(new_start, len);
                throw;
            }  
            mystl::destroy(start_, finish_);
            data_allocator::deallocate(start_, old_size);
            start_ = new_start;
            finish_ = new_finish;
            end_of_storage_ = new_start + len;
        }
    }
};



// reserve函数，预留空间大小，当原容量小于要求大小时，才会重新分配
template <class T>
void vector<T>::reserve(size_type n) {
    if(capacity() < n) {
        THROW_LENGTH_ERROR_IF((!(n <= max_size())), "can not larger than max_size in vector<T>::reserve."); //保证n合法
        const size_type old_size = size();
        iterator tmp = data_allocator::allocate(n);
        mystl::uninitialized_move(start_, finish_, tmp); // 移动到新的内存，原有的对象被析构了，因为move
        data_allocator::deallocate(start_);
        start_ = tmp;
        finish_ = tmp + old_size;
        end_of_storage_ = tmp + n;
    }
}

// 放弃多余的容量，让finish_ == end_of_storage_
template <class T>
void vector<T>::shrink_to_fit() {
    if(finish_ < end_of_storage_) {
        int sz = size();
        iterator new_start = data_allocator::allocate(sz); //重新分配size大小的空间
        try{
            mystl::uninitialized_move(start_, finish_, new_start);
        } catch(...) {
            // 有异常就把新的空间释放
            data_allocator::deallocate(new_start, sz);
            throw;
        }
        data_allocator::deallocate(start_, finish_ - start_);
        start_ = new_start;
        finish_ = new_start + sz;
        end_of_storage_ = finish_;
    }
}

/* void push_back(const value_type& value);
    void push_back(value_type&& value_type);

    void pop_back(); */
template <class T>
void vector<T>::push_back() {
    push_back(value_type());
}

template <class T>
void vector<T>::push_back(const value_type& value) {
    if(finish_ < end_of_storage_) {
        // 还有空间
        mystl::construct(finish_, value);
        ++finish_;
    }else {
        insert_aux(finish_, value);
    }
}

template <class T>
void vector<T>::pop_back() {
    MYSTL_DEBUG(!empty()); //保证非空
    --finish_;
    mystl::destroy(finish_);
}

// 向指定位置插入一个value
template <class T>
typename vector<T>::iterator 
vector<T>::insert(iterator pos, const value_type& value) {
    size_type n = pos - start_;
    if(finish_ != end_of_storage_ && pos == end()) {
        // push_back 且不用扩容的情况
        mystl::construct(pos, value);
        ++finish_;
    }else{
        insert_aux(pos, value);
    }
    return start_ + n; // 一定不能返回pos，有可能内存移动了，迭代器失效
}

template <class T>
void vector<T>::insert(iterator pos, size_type n, const value_type& value) {
    fill_insert(pos, n, value);
}

template <class T>
template <class Iterator>
void vector<T>::insert(iterator pos, Iterator first, Iterator last) {
    typedef typename is_integral<Iterator>::value is_Int;
    insert_range(pos, first, last, is_Int());
}



// 重新设置大小，跟assign的区别就是，assign是所有值都要设置为value
// 而resize是多出来的才设置为value
template <class T>
void vector<T>::resize(size_type new_size) {
    resize(new_size, T());
}

template <class T>
void vector<T>::resize(size_type new_size, const value_type& value) {
    if(new_size < size()) {
        erase(begin() + new_size, end());
    }else{
        // 需要插入
        insert(end(), new_size - size(), value); // size value
    }
}

template <class T>
typename vector<T>::iterator 
vector<T>::erase(iterator pos) {
    iterator i = mystl::copy(pos + 1, finish_, pos);
    mystl::destroy(i, finish_);
    finish_ = i;
    return pos;
}

template <class T>
typename vector<T>::iterator 
vector<T>::erase(iterator first, iterator last) {
    iterator i = mystl::copy(last, finish_, first);
    mystl::destroy(i, finish_);
    finish_ = i;
    return first;
}

// -------------------------重载比较操作符------------------------------
template <class T>
inline bool operator==(const vector<T>& lhs, const vector<T>& rhs) {
    return lhs.size() == rhs.size() && 
            mystl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T>
inline bool operator!=(const vector<T>& lhs, const vector<T>& rhs) {
    return !(lhs == rhs);
}

// 定义<，就能知道其他所有的情况
template <class T>
inline bool operator<(const vector<T>& lhs, const vector<T>& rhs) {
    return mystl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <class T>
inline bool operator>=(const vector<T>& lhs, const vector<T>& rhs) {
    return !(lhs < rhs);
}

template <class T>
inline bool operator>(const vector<T>& lhs, const vector<T>& rhs) {
    return rhs < lhs;
}

template <class T>
inline bool operator<=(const vector<T>& lhs, const vector<T>& rhs) {
    return !(lhs > rhs);
}

} // namespace std

#endif