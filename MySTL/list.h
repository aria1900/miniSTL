#ifndef __LIST_H__
#define __LIST_H__

#include <initializer_list>

#include "allocator.h"
#include "construct.h"
#include "algobase.h"
#include "exceptdef.h"
#include "iterator.h"
#include "type_traits.h"
#include "uninitialized.h"
#include "functional.h"


namespace mystl {

// 定义list的结点，双向链表，采用单层设计
template <class T>
struct list_node {
    list_node<T>* prev_;
    list_node<T>* next_;
    T data_;
};

// 迭代器的设计
template <class T>
struct list_iterator {
    // 迭代器的big-five 都是针对迭代器指向的元素本身的特性
    typedef bidirectional_iterator_tag iterator_category; //双向迭代器
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef T& reference;

    typedef size_t size_type;
    typedef list_node<T> Node;          //结点型别定义

    typedef list_iterator<T> iterator;
    typedef list_iterator<const T> const_iterator;
    typedef list_iterator<T> self;

    Node* node_;          

    // 迭代器的构造函数，支持默认，传入node指针，和拷贝构造
    list_iterator() = default;
    list_iterator(Node* x) : node_(x) {}
    list_iterator(const iterator& x) : node_(x.node_) {}

    void incr() { node_ = node_->next_; } // 移动到下一个node
    void decr() { node_ = node_->prev_; } // 移动到上一个node

    reference operator*() const { return node_->data_; }  
    pointer operator->() const { return &(operator*()); }

    self& operator++() {
        this->incr();
        return *this;
    }

    self operator++(int) {
        self tmp = *this;
        this->incr();
        return *this;
    }

    self& operator--() {
        this->decr();
        return *this;
    }

    self operator--(int) {
        self tmp = *this;
        this->decr();
        return *this;
    }

    // 迭代器要提供 == !=
    bool operator==(const self& rhs) const { return node_ == rhs.node_; }
    bool operator!=(const self& rhs) const { return node_ != rhs.node_; }
};


// 模板类 list 模板参数T代表容器内的数据类型
template <class T>
class list {

public:
    // list的内置型别定义

    // 有关配置器的
    typedef mystl::allocator<T> allocator_type;
    typedef mystl::allocator<T> data_allocator;
    typedef mystl::allocator<list_node<T>> node_allocator;
    allocator_type get_allocator() { return node_allocator(); } //返回的是Node类型的分配器

    // 有关容器元素的
    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;

    // 有关容器迭代器的
    typedef list_iterator<T> iterator;
    typedef list_iterator<const T> const_iterator;
    typedef mystl::reverse_iterator<iterator> reverse_iterator;
    typedef mystl::reverse_iterator<const_iterator> const_reverse_iterator;

    // 结点
    typedef list_node<T> Node;
    typedef list_node<T>* node_ptr;
    typedef list_node<const T>* const_node_ptr;

private:
    // 成员变量
    node_ptr node_; //指向一个node结点，整体结构为环形链表，这个有点像虚拟结点

public:
    // 构造、移动、拷贝、赋值和析构函数
    list() {
        fill_init(0, value_type());
    }
    
    explicit list(size_type n) {
        fill_init(n, value_type());
    }

    list(size_type n, const T& value) {
        fill_init(n, value);
    }

    template <class Iterator>
    list(Iterator first, Iterator last) {
        typedef typename is_integral<Iterator>::value is_Int;
        range_init_aux(first, last, is_Int());
    }

    list(std::initializer_list<T> ilist) {
        copy_init(ilist.begin(), ilist.end());
    }

    list(const list& rhs) {
        copy_init(rhs.begin(), rhs.end());
    }

    // 抢夺资源，并把原对象置空
    list(list&& rhs) noexcept : node_(rhs.node_) {
        rhs.node_ = nullptr;
    }

    list& operator=(const list& rhs) {
        if(this != &rhs) {
            //判断自复制
            assign(rhs.begin(), rhs.end());
        }
        return *this;
    }

    list& operator=(list&& rhs) {
        clear();
        splice(end(), rhs); //把rhs的资源移动到end后面
        return *this;
    }

    list& operator=(std::initializer_list<T> ilist) {
        list tmp(ilist);
        swap(tmp);
        return *this;
    }

    ~list() {
        if(node_) {
            clear();
            node_allocator::deallocate(node_);
            node_ = nullptr;
        }
    }

public:
    // 迭代器相关操作，这里const相关的，必须要用list_node<const T>*来强转
    // 否则Node并不是指向const T的，编译器会报错
    iterator begin() { return node_->next_; } 
    const_iterator begin() const { return (const_node_ptr)node_->next_; }
    iterator end() { return node_; }
    const_iterator end() const { return (const_node_ptr)node_; }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return reverse_iterator(begin()); }

    const_iterator cbegin() const { return begin(); } 
    const_iterator cend() const { return end(); }
    const_reverse_iterator crbegin() const { return rbegin(); }
    const_reverse_iterator crend() const { return rend(); }

    // 容量相关操作
    bool empty() const {
        return node_->next_ == node_;
    }

    size_type size() const {
        size_type result = distance(begin(), end());
        return result;
    }

    size_type max_size() const {
        return static_cast<size_type>(-1);
    }

    // 元素访问相关操作
    reference front() {
        MYSTL_DEBUG(!empty());
        return *begin();
    }

    const_reference front() const {
        MYSTL_DEBUG(!empty());
        return *begin();
    }

    reference back() {
        MYSTL_DEBUG(!empty());
        return *(--end());
    }

    const_reference back() const {
        MYSTL_DEBUG(!empty());
        return *(--end());
    }

    // 调整容器相关操作

    // assign
    void assign(size_type n, const value_type* value) {
        fill_assign(n, value);
    }
    
    template <class Iterator>
    void assign(Iterator first, Iterator last) {
        typedef typename is_integral<Iterator>::value is_Int;
        range_assign_aux(first, last, is_Int());
    }

    void assign(std::initializer_list<T> ilist) {
        copy_assign(ilist.begin(), ilist.end());
    }

    // insert
    iterator insert(iterator pos, const value_type& value);

    void insert(iterator pos, size_type n, const value_type& value) {
        fill_insert(pos, n, value);
    }

    template <class Iterator>
    void insert(iterator pos, Iterator first, Iterator last) {
        typedef typename is_integral<Iterator>::value is_Int;
        range_insert_aux(pos, first, last, is_Int());
    }

    // push_front / push_back
    void push_front(const value_type& value) {
        insert(begin(), value);
    }

    void push_back(const value_type& value) {
        insert(end(), value);
    }

    // pop_front / pop_back
    void pop_front() {
        MYSTL_DEBUG(!empty());
        erase(begin());
    }

    void pop_back() {
        MYSTL_DEBUG(!empty());
        iterator tmp = end();
        erase(--tmp);
    }

    // erase
    iterator erase(iterator pos);

    iterator erase(iterator first, iterator last);

    // clear
    void clear();

    // resize
    void resize(size_type new_size) { resize(new_size, value_type()); }
    void resize(size_type new_size, const value_type& value);

    // list相关操作 swap splice remove unique merge sort reverse
    void swap(list& rhs) {
        mystl::swap(node_, rhs.node_);
    }

    void splice(iterator pos, list& other);
    void splice(iterator pos, list& other, iterator i); //将other的i转移到pos之前
    void splice(iterator pos, list& other, iterator first, iterator last);

    // remove,移除符合条件的元素
    void remove(const value_type& value);
    
    template <class UnaryPredicate>
    void remove_if(UnaryPredicate pred);

    // unique,去除相邻的相同的元素，剩下第一个
    void unique();
    
    template <class BinaryPredicate>
    void unique(BinaryPredicate pred);

    // merge, 合并两个链表，从小到大归并,必须保证有序
    void merge(list& x);

    template <class BinaryPredicate>
    void merge(list& x, BinaryPredicate comp);  //自定义opreator<

    // sort，由于全局sort是针对random_iterator，所以list内部提供一个sort
    void sort();

    template <class Compare>
    void sort(Compare comp);

    // reverse，反转练表
    void reverse();
private:
    // fill_xxx 一般是n value形式的
    // copy_xxx 一般是first last形式的
    // range_xxx_aux 一般是控制分支的

    // 辅助函数
    node_ptr create_node(const value_type& value);
    void destroy_node(node_ptr ptr);


    // 初始化n个value的结点
    void fill_init(size_type n, const value_type& value);

    // 以[first, last) 初始化容器
    template <class Iterator>
    void copy_init(Iterator first, Iterator last);

    template <class Integer>
    void range_init_aux(Integer n, Integer value, true_type) {
        fill_init(n, value); // 还是调用 n value 格式的
    }

    template <class Iterator>
    void range_init_aux(Iterator first, Iterator last, false_type) {
        copy_init(first, last); // iterator 格式的
    }

    // 在尾部连接[first, last] 结点,左闭右闭
    void link_nodes_at_back(node_ptr first, node_ptr last) {
        last->next_ = node_;
        first->prev_ = node_->prev_;
        node_->prev_->next_ = first;
        node_->prev_ = last;
    }

    // assign的辅助函数
    void fill_assign(size_type n, const value_type& value);

    template <class Iterator>
    void copy_assign(Iterator first, Iterator last);

    template <class Integer>
    void range_assign_aux(Integer n, Integer value, true_type) {
        fill_assign(n, value);
    }

    template <class Iterator>
    void range_assign_aux(Iterator first, Iterator last, false_type) {
        copy_assign(first, last);
    }

    // insert辅助函数
    void fill_insert(iterator pos, size_type n, const value_type& value);

    template <class Iterator>
    void copy_insert(iterator pos, Iterator first, Iterator last);

    template <class Integer>
    void range_insert_aux(iterator pos, Integer n, Integer value, true_type) {
        fill_insert(pos, n, value);
    }

    template <class Iterator>
    void range_insert_aux(iterator pos, Iterator first, Iterator last, false_type) {
        copy_insert(pos, first, last);
    }

    // transfer 将[first, last)转移到pos之前
    void transfer(iterator pos, iterator first, iterator last);

    // 归并排序sort的辅助函数
    template <class Compare>
    iterator sort_aux(iterator first1, iterator last2, size_type n, Compare comp);
};

// 创造一个结点，并构造对象，返回其指针
template <class T>
typename list<T>::node_ptr 
list<T>::create_node(const value_type& value) {
    node_ptr tmp = node_allocator::allocate(1);
    try{
        mystl::construct(&tmp->data_, value); //构造对象
    } catch(...) {
        node_allocator::deallocate(tmp);
    }
    
    return tmp;
}

// 销毁一个结点，析构对象
template <class T>
void list<T>::destroy_node(node_ptr ptr) {
    mystl::destroy((T*)&ptr->data_);
    node_allocator::deallocate(ptr);
}

template <class T>
void list<T>::fill_init(size_type n, const value_type& value) {
    // 先创造一个node结点，也就是dummy结点
    node_ = create_node(value_type(0)); //值随意
    node_->next_ = node_;    //都指向自己
    node_->prev_ = node_;

    try{
        for(; n > 0 ; --n) {
            node_ptr new_node = create_node(value);
            link_nodes_at_back(new_node, new_node); // 把该结点连接到back
        }
    }catch(...) {
        // 初始化失败
        clear();
        node_allocator::deallocate(node_);
        node_ = nullptr;
        throw;
    }
}

template <class T>
template <class Iterator>
void list<T>::copy_init(Iterator first, Iterator last) {
    node_ = create_node(value_type(0)); //值随意
    node_->next_ = node_;    //都指向自己
    node_->prev_ = node_;

    size_type n = mystl::distance(first, last);
    try{
        for(; n > 0 ; --n, ++first) {
            node_ptr new_node = create_node(*first);
            link_nodes_at_back(new_node, new_node);
        }
    }catch(...) {
        clear();
        node_allocator::deallocate(node_);
        node_ = nullptr;
        throw;
    }
}

template <class T>
void list<T>::fill_assign(size_type n, const value_type& value) {
    iterator start = begin();
    iterator finish = end();
    for(; n > 0 && start != finish ; --n, ++start) {
        *start = value;
    }
    
    if(n > 0) {
        // 长度不够，n有剩余
        insert(finish, n, value); //结尾插入剩余n个value
    }else {
        //多余了
        erase(start, finish);
    }
}

template <class T>
template <class Iterator>
void list<T>::copy_assign(Iterator first, Iterator last) {
    iterator start = begin();
    iterator finish = end();
    for(; first != last && start != finish ; ++first, ++start) {
        *start = *first;
    }
    if(first != last) {
        // 长度不够
        insert(finish, first, last);
    }else {
        // 多余
        erase(start, finish);
    }
}

template <class T>
typename list<T>::iterator 
list<T>::insert(iterator pos, const value_type& value) {
    //THROW_LENGTH_ERROR_IF(size() > max_size() - 1, "list<T>'s size too big.\n");  // 每次检测size O(n)
    
    /* node_ptr new_node = node_allocator::allocate(1); //分配一个结点的空间
    mystl::construct(&(new_node->data_), value); */

    node_ptr new_node = create_node(value);

    // 连接到链上
    new_node->next_ = pos.node_;
    new_node->prev_ = pos.node_->prev_;

    // 将前后指针指向new_node
    pos.node_->prev_->next_ = new_node;
    pos.node_->prev_ = new_node;
    return new_node;
}

template <class T>
void list<T>::fill_insert(iterator pos, size_type n, const value_type& value) {
    for(; n > 0 ; --n) {
        insert(pos, value); //插入n个value
    }
}

// 在pos之前插入[first, last),依次插入first-->last在pos之前即可
template <class T>
template <class Iterator>
void list<T>::copy_insert(iterator pos, Iterator first, Iterator last) {
    for(; first != last ; ++first) {
        insert(pos, *first);
    }
}

template <class T>
typename list<T>::iterator
list<T>::erase(iterator pos) {
    iterator tmp = pos.node_->next_;
    pos.node_->prev_->next_ = pos.node_->next_;
    pos.node_->next_->prev_ = pos.node_->prev_;
    /* mystl::destroy(&(pos.node_->data_));
    node_allocator::deallocate(pos.node_); */
    destroy_node(pos.node_); //传入node_ptr
    return tmp;
}

template <class T>
typename list<T>::iterator
list<T>::erase(iterator first, iterator last) {
    for(; first != last ;) {
        first = erase(first); //防止迭代器失效
    }
    return first;
}

template <class T>
void list<T>::clear() {
    if(size() != 0) {
        node_ptr cur = node_->next_; //第一个结点
        for(node_ptr next = cur->next_ ; cur != node_ ; 
            cur = next, next = cur->next_) {
            destroy_node(cur);
        }
        node_->next_ = node_;
        node_->prev_ = node_;
    }
}

template <class T>
void list<T>::resize(size_type new_size, const value_type& value) {
    iterator i = begin();
    size_type len = 0;
    for(; i != end() && len < new_size ; ++i, ++len);
    if(len == new_size) {
        // 说明多余了
        erase(i, end());
    }else{
        // 少了需要insert
        insert(i, new_size - len, value);
    }
}

template <class T>
void list<T>::transfer(iterator pos, iterator first, iterator last) {
    // pos == last 相当于不操作
    if(pos != last) {
        // 将原来的链上的[first, last)去除
        node_ptr tmp = last.node_->prev_; //记录last的前一个结点
        first.node_->prev_->next_ = last.node_;
        last.node_->prev_ = first.node_->prev_;

        // 连接到新的位置
        tmp->next_ = pos.node_;
        first.node_->prev_ = pos.node_->prev_;
        pos.node_->prev_->next_ = first.node_;
        pos.node_->prev_ = tmp;
    }
}

// 都是在list内部进行操作的
template <class T>
void list<T>::splice(iterator pos, list& other) {
    if(!other.empty()) {
        // 非空才操作
        transfer(pos, other.begin(), other.end());
    }
}

template <class T>
void list<T>::splice(iterator pos, list& /* other */, iterator i) {
    iterator j = i;
    ++j;
    if(pos == i || pos == j) return; //这两个都是不移动的
    transfer(pos, i, j);
}

template <class T>
void list<T>::splice(iterator pos, list& /* other */, iterator first, iterator last) {
    if(first != last) {
        // 非空
        transfer(pos, first, last);
    }
}

template <class T>
void list<T>::remove(const value_type& value) {
    iterator first = begin();
    iterator last = end();
    for(; first != last ; ) {
        if(*first == value) {
            first = erase(first);
        }else {
            ++first;
        }
    }
}

template <class T>    
template <class UnaryPredicate>
void list<T>::remove_if(UnaryPredicate pred) {
    iterator first = begin();
    iterator last = end();
    for(; first != last ; ) {
        if(pred(*first)) {
            first = erase(first);
        }else {
            ++first;
        }
    }
}

template <class T>
void list<T>::unique() {
    iterator first = begin();
    iterator last = end();
    if(first == last) return; //空
    iterator next = first;
    while(++next != last) {
        if(*first == *next) {
            erase(next);
        }else{
            first = next; //否则移动
        }
        next = first; //不管是擦除了还是没有擦除，next = first，这样不会迭代器失效
    }
}

template <class T>
template <class BinaryPredicate>
void list<T>::unique(BinaryPredicate pred) {
    iterator first = begin();
    iterator last = end();
    if(first == last) return; 
    iterator next = first;
    while(++next != last) {
        if(pred(*first, *next)) {
            erase(next);
        }else{
            first = next; 
        }
        next = first; 
    }
}

template <class T>
void list<T>::merge(list& x) {
    iterator first1 = begin();
    iterator last1 = end();
    iterator first2 = x.begin();
    iterator last2 = x.end();

    while(first1 != last1 && first2 != last2) {
        if(*first2 < *first1) {
            iterator next = first2;  //将first2移动到first1之前
            transfer(first1, first2, ++next);
            first2 = next; //防止迭代器失效
        }else {
            // *first1 < *first2 没有到插入的位置
            ++first1;
        }
    }
    // 如果first2 != last2 说明后面的数都大于last1 则整体搬到后面去
    if(first2 !=last2) transfer(last1, first2, last2);
}

template <class T>
template <class BinaryPredicate>
void list<T>::merge(list& x, BinaryPredicate comp) {
    iterator first1 = begin();
    iterator last1 = end();
    iterator first2 = x.begin();
    iterator last2 = x.end();

    while(first1 != last1 && first2 != last2) {
        if(comp(*first2, *first1)) {
            iterator next = first2;
            transfer(first1, first2, ++next);
            first2 = next;
        }else {
            ++first1;
        }
    }
    if(first2 !=last2) transfer(last1, first2, last2);
}

// 双链表直接交换prev和next指针即可，但是单链表不能这样做，因为找不到前驱结点
template <class T>
void list<T>::reverse() {
    node_ptr cur = node_;
    do{
        mystl::swap(cur->next_, cur->prev_);
        cur = cur->prev_; //移动到下一个结点
    } while(cur != node_);
}

// 将长度为n的[first1, last2)区间排序，并返回最小值的结点iterator
// 因为还要将[first1, last2) 分割为[first1, last1) [first1, last2),所以这样命名
template <class T>
template <class Compare>
typename list<T>::iterator 
list<T>::sort_aux(iterator first1, iterator last2, size_type n, Compare comp) {
    if(n == 1) return first1; //长度为1直接返回
   
    size_type len = n / 2;
    iterator last1 = first1;
    mystl::advance(last1, len); //last1移动到中间
    iterator first2 = last1;

    // 递归排序
    first1 = sort_aux(first1, last1, len, comp);
    first2 = sort_aux(first2, last2, n - len, comp);

    // 开始归并left和right，连续的两段，由于在同一个list内，可能迭代器失效
    // 和merge函数不一样
    last1 = first2;
    last2  = first2;
    mystl::advance(last2, n - len);

    /* std::cout << "left: ";
    iterator cur1 = first1;
    while(cur1 != last1) {
        std::cout << *cur1 << " ";
        ++cur1;
    }

    std::cout << "right: ";
    iterator cur2 = first2;
    while(cur2 != last2) {
        std::cout << *cur2 << " ";
        ++cur2;
    }
    std::cout << std::endl; */

    iterator head = comp(*first2, *first1) ? first2 : first1;

    while(first1 != last1 && first2 != last2) {
        if(comp(*first2, *first1)) {
            // *first2 < *first1
            iterator next = first2;
            transfer(first1, first2, ++next);
            first2 = next;
            last1 = next; //last1 == first2 因此也需要变 
        }else{
            ++first1;
        }
    }
    /* std::cout << "after sort : ";
    iterator cur = head;
    while(cur != last2) {
        std::cout << *cur << " ";
        ++cur;
    }
    std::cout << std::endl; */

    return head;
}

template <class T>
void list<T>::sort() {
    sort_aux(begin(), end(), size(), mystl::less<T>());
}

template <class T>
template <class Compare>
void list<T>::sort(Compare comp) {
    sort_aux(begin(), end(), size(), comp);
}


}


#endif