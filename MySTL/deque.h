#ifndef __DEQUE_H__
#define __DEQUE_H__

#include <stddef.h>

#include "allocator.h"
#include "construct.h"
#include "algobase.h"
#include "exceptdef.h"
#include "iterator.h"
#include "type_traits.h"
#include "uninitialized.h"
#include "functional.h"
#include "util.h"
// 这个文件定义了deque类，以及相关的方法


/*
* 关于deque的一些难点在这里写一下。
*
* 首先是迭代器的设计，迭代器包含4个指针，3个T*，和一个T**。分别指向buffer的起始、结束和当前元素位置，另一个T**指向控制buffer的map的位置。
* 迭代器的难点就是operator+=的设计，首先采用offset来表示移动后的位置离当前buffer差多少个位置，当然offset也可能为负。
* 为正的时候，就简单的 offset / buffer_size，就是移动的node数量，之后对其取模就是cur的移动的位置
* 为负的时候， - (- offset - 1) / buff_size - 1， 第二个-1就是保证比如offset == -3的时候移动node为0的情况，第一个-1保证offset == n * buffer_size，会多移一个buffer

* 对于deque的难点，这里有好几点。
* 初始化的函数create_map_and_nodes(size_type n)，该函数为n个元素来创建map，并根据n的不同来创建buffer，并连接到map，最少创建一个。同时保证buffer在中间。
*
* 另一个就是有关map重新分配的一个函数，require_capacity(size_type n, bool front。该函数保证头部或者尾部有n个空间。
* 如果map的空位所能创建的buffer，足够n个元素填充，直接create_nodes。如果不够，则调用reallocate_map_at_back(front)，传入的是还需要buffer的数量，
* 根据这个来进行map_allocator::allocate(new_map_size)，并且将new_start重新调整，把旧的buffer连接到相应的位置，同时新的位置上创建buffer。
* 注意这个函数仅仅保证buffer的足够，对于对象的构造是不会管的，也就是说创建的是未初始化的内存。
*
* 对于插入的一些操作，在保证空间足够的情况下，需要对元素进行移动，但是具体移动pos前面的还是pos后面的，得看哪边元素少，用(elems_before < (len / 2))来判断。
* 有关移动的操作，需要进行(elems_before < n)这样的判断，具体来对未初始化的内存和已初始化的内存进行操作。
*/



namespace mystl {

// 定义map的初始化大小为8
#ifndef DEQUE_MAP_INIT_SIZE
#define DEQUE_MAP_INIT_SIZE 8
#endif

// 根据T的类型来决定buf的个数有多少个
template <class T>
struct deque_buf_size{
    static constexpr size_t value = sizeof(T) < 256 ? 4096 / sizeof(T) : 16; //太大就定义为16个
};

// deque迭代器的设计
template <class T, class Ref, class Ptr>
struct deque_iterator {
    // 有关iterator的定义
    typedef deque_iterator<T, T&, T*> iterator;
    typedef deque_iterator<T, const T&, const T*> const_iterator;
    typedef deque_iterator self;

    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef Ptr pointer;    // T*
    typedef Ref reference;  // T&
    typedef size_t size_type;
    typedef ptrdiff_t difference_type; 
    typedef T* value_pointer;   // 指向buf的值
    typedef T** map_pointer;    // 指向map的位置

    static const size_type buffer_size = deque_buf_size<T>::value; // static变量，buf内部的数量。sgi-stl是用static函数设计的

    // 迭代器的4个指针，保证了对外部提供random的性质的可能
    value_pointer cur;      // 指向当前元素
    value_pointer first;    // 指向当前buf的头部
    value_pointer last;     // 指向当前buf的尾部（右开）
    map_pointer node;       // 指向buf在map中的位置

    // 构造、复制、赋值函数
    deque_iterator() : cur(nullptr), first(nullptr), last(nullptr), node(nullptr) {}

    deque_iterator(value_pointer v, map_pointer n) : cur(v), first(*n), last(*n + buffer_size), node(n) {} // 提供cur和node的指针，first通过*n来获取

    deque_iterator(const iterator& rhs) : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}

    deque_iterator(iterator && rhs) : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {
        // 释放rhs资源
        rhs.cur = nullptr;
        rhs.first = nullptr;
        rhs.last = nullptr;
        rhs.node = nullptr;
    }

    deque_iterator(const const_iterator& rhs) : cur(rhs.cur), first(rhs.first), last(rhs.last), node(rhs.node) {}

    self& operator=(const iterator& rhs) {
        if(this != &rhs) {
            cur = rhs.cur;
            first = rhs.first;
            last = rhs.last;
            node = rhs.node;
        }
        return *this;
    }

    // 重载运算符
    reference operator*() const { return *cur ; }
    pointer operator->() const { return cur; }

    // *this - x 的情况
    difference_type operator-(const self& x) const {
        return difference_type(buffer_size) * (node - x.node - 1)  // 这里是求完整的一块buf所以要-1
               + (cur - first) + (x.last - x.cur);
    }

    self& operator++() {
        ++cur;
        if(cur == last) {
            set_node(node + 1);
            cur = first; //set_node 并没有设置cur
        }
        return *this;
    }

    self operator++(int) {
        self tmp = *this;
        ++*this;
        return tmp;
    }

    self& operator--() {
        if(cur == first) {
            set_node(node - 1);
            cur = last;
        }
        --cur;
        return *this;
    }

    self operator--(int) {
        self tmp = *this;
        --*this;
        return tmp;
    }

    // DEBUG : 在offset < 0时，忘记添加负号，导致迭代器移动失败
    // operator+= 对于该迭代器对外呈现的random的性质都是通过这个来实现的，很重要
    self& operator+=(difference_type n) {
        difference_type offset = n + (cur - first);     //目标距离first的偏移
        if(offset >= 0 && offset < static_cast<difference_type>(buffer_size)) {
            // 在当前缓冲区内
            cur += n;
        }else {
            // 跳到其他缓冲区内
            // 关于offset为正不难理解
            // offset为负的时候，这时候第二个减号是做修正左右的，比如向后移动3个位置，且cur在first，算下来为0，修正后为-1
            // 第一个减号是配合第二个减号使用的，如刚好向后移动两个buffer_size，算下来为-2，再减一个1，则为-3，不符合
            // 所以要在绝对值|offset|减一，做一个小小的调整
            difference_type node_offset = offset > 0 
            ? offset / static_cast<difference_type>(buffer_size)     // offset > 0
            : -static_cast<difference_type>( (-offset - 1) / buffer_size ) - 1;

            set_node(node + node_offset); // node_offset 有正有负
            cur = first + (offset - node_offset * static_cast<difference_type>(buffer_size)); //将cur移动到正确的位置
        }
        return *this;
    }

    self operator+(difference_type n) const {
        self tmp = *this;
        return tmp += n;
    }

    self& operator-=(difference_type n) {
        return *this += -n;
    } 

    self operator-(difference_type n) {
        self tmp = *this;
        return tmp += -n;
    }

    reference operator[] (difference_type n) const { return *(*this + n); }  

    // 重载比较运算符
    bool operator==(const self& rhs) const { return cur == rhs.cur; }

    bool operator!=(const self& rhs) const { return !(*this == rhs); }

    bool operator<(const self& rhs) const {
        return node == rhs.node ? (cur < rhs.cur) : (node < rhs.node);
    }

    bool operator>=(const self& rhs) const { return !(*this < rhs); }

    bool operator>(const self& rhs) const { return rhs < *this; }

    bool operator<=(const self& rhs) const { return !(*this > rhs); }

    // 转移到另一个缓冲区，注意不包含cur的设置
    void set_node(map_pointer new_node) {
        node = new_node;
        first = *node;
        last = *node + difference_type(buffer_size);
    }
};


// 模板类 deque
template <class T>
class deque {
public:
    // allocator的型别定义
    typedef mystl::allocator<T> allocator_type;
    typedef mystl::allocator<T> data_allocator;
    typedef mystl::allocator<T*> map_allocator;

    typedef T value_type;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef size_t size_type;
    typedef ptrdiff_t difference_type;
    typedef pointer* map_pointer;
    typedef const_pointer* const_map_pointer;

    typedef deque_iterator<T, T&, T*> iterator;
    typedef deque_iterator<T, const T&, const T*> const_iterator;
    typedef mystl::reverse_iterator<iterator> reverse_iterator;
    typedef mystl::reverse_iterator<const_iterator> const_reverse_iterator;

    allocator_type get_allocator() { return allocator_type(); }

    static const size_type buffer_size = deque_buf_size<T>::value; // 同迭代器的buffer_size保持一致

private:
    // 真正的成员变量，用四个数据体现一个deque
    iterator start_;        // 指向第一个buf，迭代器，又包括了4个指针
    iterator finish_;       // 指向最后一个buf
    map_pointer map_;       // 指向中控器map
    size_type map_size_;    // map的大小

// debug使用
public:
    void debugFunc() {
        std::cout << "map_size : " << map_size_ << std::endl;
        std::cout << "size : " << size() << std::endl;
        std::cout << "nodes num : " << finish_.node - start_.node + 1 << std::endl;
    }

public:
    // 构造、复制、移动、析构函数
    deque() {
        fill_init(0, value_type()); //这里包含了map和buffer的内存分配
    }

    deque(size_type n) {
        fill_init(n, value_type());
    }

    deque(size_type n, const value_type& value) {
        fill_init(n, value);
    }

    template <class Iterator>
    deque(Iterator first, Iterator last) {
        typedef typename is_integral<Iterator>::value is_Int;
        range_init_aux(first, last, is_Int());
    }

    deque(std::initializer_list<T> ilist) {
        copy_init(ilist.begin(), ilist.end());
    }

    deque(const deque& rhs) {
        copy_init(rhs.begin(), rhs.end());
    }
    
    // 将rhs的iterator转化为右值自动析构
    deque(deque&& rhs) : 
        start_(mystl::move(rhs.start_)), 
        finish_(mystl::move(rhs.finish_)),
        map_(rhs.map_),
        map_size_(rhs.map_size_)     
    {   
        // 不用将start finish的指针置空是因为在iterator的右值构造就已经做了
        rhs.map_ = nullptr;
        rhs.map_size_ = 0;
    }

    deque& operator=(const deque& rhs);
    deque& operator=(deque&& rhs);

    deque& operator=(std::initializer_list<T> ilist) {
        deque tmp(ilist); //利用构造函数来operator=
        swap(tmp);
        return *this;
    }

    ~deque() {
        if(map_ != nullptr) {
            clear();
            // 回收最后一个buffer
            data_allocator::deallocate(*start_.node, buffer_size);
            *start_.node = nullptr;
            map_allocator::deallocate(map_, map_size_);
            map_ = nullptr;
            map_size_ = 0;
        }
    }

public:
    // 迭代器相关操作
    iterator begin() { return start_; }
    const_iterator begin() const { return start_; }
    iterator end()  { return finish_; }
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
    bool empty() const { return start_ == finish_; }    // 迭代器的比较 
    size_type size() const { return finish_ - start_; } // operator-的重载
    size_type max_size() const { return static_cast<size_type>(-1); }
    
    // 扩容多余的就构造value
    void resize(size_type new_size) { resize(new_size, value_type()); }
    void resize(size_type new_size, const value_type& value);
    //void shrink_to_fit();

    // 元素访问相关操作
    reference operator[](size_type n) {
        MYSTL_DEBUG(n < size());
        return start_[n];   // iterator重载了,等价于start对象的 *(*this + n)
    }

    const_reference operator[](size_type n) const {
        MYSTL_DEBUG(n < size());
        return start_[n];
    }

    // 返回第n个元素，但是带安全检查
    reference at(size_type n) {
        THROW_OUT_OF_RANGE_IF(!(n < size()), "deque<T>::at() out of range.\n");
        return (*this)[n];  // 利用上面写好的operator[]
    }

    const_reference at(size_type n) const {
        THROW_OUT_OF_RANGE_IF(!(n < size()), "deque<T>::at() out of range.\n");
        return (*this)[n];
    }

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

public:
    // assign
    void assign(size_type n, const value_type& value) {
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

    // erase
    iterator erase(iterator pos);
    iterator erase(iterator first, iterator last);

    // push_front / push_back
    void push_front();
    void push_front(const value_type& value);

    void push_back();
    void push_back(const value_type& value);


    // pop_front / pop_back
    void pop_front();
    void pop_back();


    // 清空元素，不过会保留一个buffer
    void clear();

    // swap

    void swap(deque& rhs) {
        if(this != &rhs) {
            mystl::swap(start_, rhs.start_);
            mystl::swap(finish_, rhs.finish_);
            mystl::swap(map_, rhs.map_);
            mystl::swap(map_size_, rhs.map_size_);
        }
    }

private:
    // 辅助函数

    // create buffer or map
    // 在[start, finish] 上创建buffer
    void create_nodes(map_pointer start, map_pointer finish);
    
    // 在[start, finish] 上销毁buffer
    void destroy_nodes(map_pointer start, map_pointer finish);

    // 初始化，为n个元素，来分配map和buffer的内存空间
    void create_map_and_nodes(size_type num_elems);


    // initialize  初始化n个value   n value 格式
    void fill_init(size_type n, const value_type& value);

    template <class Iterator>
    void copy_init(Iterator first, Iterator last);

    template <class Integer>
    void range_init_aux(Integer n, Integer value, true_type) {
        // 转调用
        fill_init(n, value);
    }

    template <class Iterator>
    void range_init_aux(Iterator first, Iterator last, false_type) {
        copy_init(first, last);
    }
    

    // assign相关
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


    // insert相关

    // 单个位置插入
    iterator insert_aux(iterator pos, const value_type& value);

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


    // reallocate 重新分配内存相关
    void require_capacity(size_type n, bool front);     // bool控制头还是尾
    void reallocate_map_at_front(size_type need);
    void reallocate_map_at_back(size_type need);
};

template <class T>
void deque<T>::create_nodes(map_pointer start, map_pointer finish) {
    map_pointer cur;
    try{
        for(cur = start ; cur <= finish ; ++cur) {
            *cur = data_allocator::allocate(buffer_size);
        }
    }catch(...) {
        while(cur != start) {
            --cur;
            data_allocator::deallocate(*cur, buffer_size);
            *cur = nullptr; //指向的指针置空
        }
    }
}

template <class T>
void deque<T>::destroy_nodes(map_pointer start, map_pointer finish) {
    map_pointer cur;
    // 仅仅回收buffer的内存，map的内存先保留
    for(cur = start ; cur <= finish ; ++cur) {
        data_allocator::deallocate(*cur, buffer_size);
        *cur = nullptr;
    }
}

// 创建map和buffer，指针的调整很重要
template <class T>
void deque<T>::create_map_and_nodes(size_type num_elems) {
    size_type num_nodes = num_elems / buffer_size + 1; // 如果刚好整除则多分配一个，记住这里的多分配一个
    map_size_ = mystl::max(static_cast<size_type>(DEQUE_MAP_INIT_SIZE), num_nodes + 2); //要么多分配2个，或者直接分配8个
    map_ = map_allocator::allocate(map_size_);      //分配map内存
    map_pointer nstart = map_ + (map_size_ - num_nodes) / 2;  //让nstart指向map的中间位置
    map_pointer nfinish = nstart + num_nodes - 1;              // 注意这里的减1，如果没有整除，就正好指向最后一个buffer。如果整除了，就指向多分配的那个buffer

    try {
        create_nodes(nstart, nfinish);
    }catch(...) {
        map_allocator::deallocate(map_, map_size_); //销毁map
        map_ = nullptr;
        map_size_ = 0;
        throw;
    }
    start_.set_node(nstart);
    finish_.set_node(nfinish);
    start_.cur = start_.first;
    finish_.cur = finish_.first + (num_elems % buffer_size);    // 如果刚好整除那就是0，指向多分配的buffer起始
}

template <class T>
void deque<T>::fill_init(size_type n, const value_type& value) {
    create_map_and_nodes(n);    // 即使n==0，也会分配buffer和map

    // 填充元素
    if(n != 0) {
        for(map_pointer cur = start_.node ; cur < finish_.node ; ++cur) {
            mystl::uninitialized_fill(*cur, *cur + buffer_size, value); //每一个buffer都填满value
        }

        // finish_ 的部分单独处理
        mystl::uninitialized_fill(finish_.first, finish_.cur, value);   //如果刚好整除，这里不用填
    }
}

template <class T>
template <class Iterator>
void deque<T>::copy_init(Iterator first, Iterator last) {
    const size_type n = mystl::distance(first, last); //n个元素
    create_map_and_nodes(n);    //分配内存
    for(map_pointer cur = start_.node ; cur < finish_.node ; ++cur) {
        Iterator next = first;
        mystl::advance(next, buffer_size); //朝后面移动buffer_size个位置
        mystl::uninitialized_copy(first, next, *cur);
        first = next;
    }
    // finish的位置单独处理
    mystl::uninitialized_copy(first, last, finish_.first);
}


// operator=

template <class T>
deque<T>& deque<T>::operator=(const deque& rhs) {
    if(this != &rhs) {
        const size_type len = size();
        if(len >= rhs.size()) {
            // 多了需要擦除
            erase(mystl::copy(rhs.start_, rhs.finish_, start_), finish_);
        }else {
            // rhs比较多，需要额外插入
            iterator mid = rhs.start_ + len;
            mystl::copy(rhs.start_, mid, start_);
            insert(finish_, mid, rhs.finish_);
        }
    }
    return *this;
}


template <class T>
deque<T>& deque<T>::operator=(deque&& rhs) {
    clear();    // 清空自身的资源
    start_ = mystl::move(rhs.start_);
    finish_ = mystl::move(rhs.finish_);
    map_ = rhs.map_;
    map_size_ = rhs.map_size_;

    // 将rhs的资源置空，由于两个迭代器在移动构造中已经置空了，所以不用管
    rhs.map_ = nullptr;
    rhs.map_size_ = 0;

    return *this;
}


// assign辅助函数
template <class T>
void deque<T>::fill_assign(size_type n, const value_type& value){
    if(size() < n) {
        // 需要insert
        mystl::fill(begin(), end(), value);
        insert(end(), n - size(), value);
    }else{
        erase(begin() + n, end());
        mystl::fill(begin(), end(), value);
    }
}

// range的则需要一个个assign
template <class T>
template <class Iterator>
void deque<T>::copy_assign(Iterator first, Iterator last) {
    iterator first1 = begin();
    iterator last1 = end();
    for(; first1 != last1 && first != last ; ++first1, ++first) {
        *first1 = *first;
    }
    if(first1 != last1) {
        // 证明自己多了
        erase(first1, last1);
    }else{
        insert(end(), first, last);
    }
}


// 清空对象，但是会保留start的buffer
template <class T>
void deque<T>::clear() {
    // 去头去尾的析构和销毁
    for(map_pointer cur = start_.node + 1 ; cur < finish_.node ; ++cur) {
        mystl::destroy(*cur, *cur + buffer_size);
        data_allocator::deallocate(*cur, buffer_size);
    }

    // 大于1个buffer的时候
    if(start_.node != finish_.node) {
        // 仅剩2个buffer
        mystl::destroy(start_.cur, start_.last);
        mystl::destroy(finish_.first, finish_.cur);
        // 不能用迭代器 *finish 因为这是finish.cur指向的对象
        data_allocator::deallocate(*finish_.node); //最后一个buffer内存回收
        *finish_.node = nullptr;
    }else {
        // 本来就只有一个buffer
        mystl::destroy(start_.cur, finish_.cur);
    }

    // 调整start finish
    finish_ = start_;
}


// resize

template <class T>
void deque<T>::resize(size_type new_size, const value_type& value) {
    const size_type len = size();
    if(new_size < len) {
        erase(start_ + new_size, finish_);
    }else{
        insert(finish_, new_size - len, value);
    }
}



// insert相关函数

/*
* 单个元素的插入采用insert_aux来完成，通过判断移动前或后元素来完成，相较于多个元素插入较简单
* 主要内存的保证由push_front和push_back来保证
*/
template <class T>
typename deque<T>::iterator  
deque<T>::insert_aux(iterator pos, const value_type& value) {
    value_type value_copy = value;
    difference_type index = pos - start_;

    if(index < (size() / 2)) {
        // 移动前端
        push_front(front());    // push_front保证内存的足够
        iterator f1 = start_;
        ++f1;   // f1 指向原有的start
        iterator f2 = f1;
        ++f2; 
        pos = start_ + index;
        ++pos;  // 新的start因为push_front变了，所以要++
        mystl::copy(f2, pos, f1);
    }else{
        push_back(back());      // 同样 push_back也保证了内存的足够
        iterator b1 = finish_;
        --b1;
        iterator b2 = b1;
        --b2;
        pos = start_ + index;   // 这里不用++，因为没有插入到前面
        mystl::copy_backward(pos, b2, b1);
    }
    *pos = value_copy;
    return pos;
}


template <class T>
typename deque<T>::iterator 
deque<T>::insert(iterator pos, const value_type& value) {
    if(pos == start_) {
        push_front(value);
        return start_;
    }else if(pos == finish_) {
        push_back(value);
        iterator tmp = finish_;
        return --tmp;
    }else {
        // 在中间插入
        return insert_aux(pos, value);
    }
}

/*
* 以下叙述多值插入的逻辑，无论是基于[n,value]的还是基于[first,last)的都是一个思路
* 首先需要判定是插入的位置前面的元素少还是后面的元素少，谁少则移动哪边的元素
* 在移动元素之前，需要先调用require_capacity(n, bool)，这个函数保证头部和尾部的buffer数量足够
* 在require_capacity中通过bool来判定是否在前端构造buffer，也就是说这个函数只管内存的扩容，并不管对象构造
* 空余的map_node不够，则依情况调用reallocate_map_at_front(back)来完成map的扩容
* 如果足够则直接在空余的node位置create_nodes，不够则依托上述函数，但这样会导致pos失效
* 在保证前面的空间足够之后，也就是require_capacity之后，通过elems_before和n比较
* 来完成具体的元素移动，因为这里两者大小关系不同的话，未初始化的内存和已经初始化的内存之间数量不一样
* 在移动元素之后，切记改变start_或finish_指针，已经出现过这样的bug

* 简而言之就是 判断移动前(后) --> 保证前后空间足够(不够则扩容) --> 元素位置移动(elems 与 n比较，会有不同的动作)

*/


template <class T>
void deque<T>::fill_insert(iterator pos, size_type n, const value_type& value) {
    const size_type elems_before = pos - start_;    // pos前面的数量
    const size_type len  = size();                  // 总数量
    value_type value_copy = value;
    if(elems_before < (len / 2)) {
        // 前面的比较少，移动前面的
        require_capacity(n, true);  // 保证前面的空间够用

        // 迭代器可能失效，因为进行了空间的调整
        iterator old_start = start_;
        iterator new_start = start_ - n;
        pos = start_ + elems_before;

        try{
            if(elems_before >= n) {
                // pos前面的元素多一点，分两步
                iterator start_n = start_ + n;
                
                mystl::uninitialized_copy(start_, start_n, new_start); //先将前面的n个copy到未初始化的内存
                start_ = new_start;
                mystl::copy(start_n, pos, old_start);       // 把剩下的elems_before - n搬到old_start那里
                mystl::fill(pos - n, pos, value_copy);           // 再填充n个value
            }else {
                // 要插入的元素n比pos前面的元素多
                mystl::uninitialized_fill(mystl::uninitialized_copy(start_, pos, new_start),
                     start_, value_copy);
                start_ = new_start;
                mystl::fill(old_start, pos, value_copy);
                debugFunc();
            }
        }catch(...) {
            if(new_start.node != start_.node) {
                destroy_nodes(new_start.node, start_.node - 1); //否则销毁新的node
            }
            throw;
        }

    }else{
        // 移动后面的
        require_capacity(n, false);     // 保证后面的空间够用
        iterator old_finish = finish_;
        iterator new_finish = finish_ + n;
        const size_type elems_after = len - elems_before;
        pos = finish_ - elems_after;

        try {
            if(elems_after > n) {
                // 后面的比较多
                iterator finish_n = finish_ - n;
                mystl::uninitialized_copy(finish_n, finish_, finish_);
                finish_ = new_finish;
                mystl::copy_backward(pos, finish_n, old_finish);
                mystl::fill(pos, pos + n,value_copy);
            }else {
                mystl::uninitialized_copy(pos, finish_, pos + n);
                finish_ = new_finish;
                mystl::uninitialized_fill(old_finish, pos + n, value_copy);
                mystl::fill(pos, old_finish, value_copy);
            }
        } catch(...) {
            if(new_finish.node != finish_.node) {
                destroy_nodes(finish_.node + 1, new_finish.node);
            }
            throw;
        }
    }
}

template <class T>
template <class Iterator>
void deque<T>::copy_insert(iterator pos, Iterator first, Iterator last) {
    const size_type elems_before = pos - start_;    // pos前面的数量
    const size_type len  = size();                  // 总数量
    const size_type n = mystl::distance(first, last);

    if(elems_before < (len / 2)) {
        require_capacity(n, true);

        iterator old_start = start_;
        iterator new_start = start_ - n;    // 插入后start的位置
        pos = start_ + elems_before;
        try{
            if(elems_before >= n) {
                iterator start_n = start_ + n;
                mystl::uninitialized_copy(start_, start_n, new_start);
                start_ = new_start;
                mystl::copy(start_n, pos, old_start);
                mystl::copy(first, last, pos - n);
            }else{
                // n比较大
                Iterator mid = first;
                mystl::advance(mid, n - elems_before);
                mystl::uninitialized_copy(first, mid, 
                    mystl::uninitialized_copy(start_,pos, new_start));
                start_ = new_start;
                mystl::copy(mid, last, old_start);
            }
        }catch(...) {
            if(new_start.node != start_.node) {
                destroy_nodes(new_start.node, start_.node - 1);
                throw;
            }
        }
    }else{
        // 尾端插入
        require_capacity(n, false);
        iterator old_finish = finish_;
        iterator new_finish = finish_ + n;
        const size_type elems_after = len - elems_before;
        pos = finish_ - elems_after;        // pos扩容会失效

        try{
            if(elems_after > n) {
                iterator finish_n = finish_ - n;
                mystl::uninitialized_copy(finish_n, finish_, finish_);
                finish_ = new_finish;
                mystl::copy_backward(pos, finish_n, old_finish);
                mystl::copy(first, last, pos);
            }else{
                Iterator mid = first;
                mystl::advance(mid, elems_after);
                mystl::uninitialized_copy(pos, finish_,
                    mystl::uninitialized_copy(mid, last, finish_)); // 这是[first, last)的后半段
                finish_ = new_finish;
                mystl::copy(first, mid, pos);
            }
        }catch(...) {
            if(new_finish.node != finish_.node) {
                destroy_nodes(finish_.node + 1, new_finish.node);
                throw;
            }
        }
    }
}



// reallocate 相关
template <class T>
void deque<T>::require_capacity(size_type n, bool front) {
    if(front && (start_.cur - start_.first) < n) {
        // 在前面添加，且添加的个数n超过剩余的空间
        const size_type need_buffer = (n - (start_.cur - start_.first)) / buffer_size + 1; // 这里一定要加一否则会不够
        if(need_buffer > (start_.node - map_)) {
            // map内的空间也不够了
            reallocate_map_at_front(need_buffer);
            return;
        }
        create_nodes(start_.node - need_buffer, start_.node - 1);
    }else if(!front && (finish_.last - finish_.cur - 1) < n) {  // -1的目的是，刚刚好够也要添加一个buffer
        // 在后面添加
        const size_type need_buffer = (n - (finish_.last - finish_.cur - 1)) / buffer_size + 1;

        if(need_buffer > (map_ + map_size_) - finish_.node - 1) {
            reallocate_map_at_back(need_buffer);
            return;
        }
        create_nodes(finish_.node + 1, finish_.node + need_buffer); // 之前的已经调整过，把接下来的buffer创建
    }
}


// DEBUG : 这里mid = begin + need，而不该是mid = begin + new_buffer，这样就会指针越界
template <class T>
void deque<T>::reallocate_map_at_front(size_type need) {
    const size_type new_map_size = mystl::max((map_size_ << 1), map_size_ + need + DEQUE_MAP_INIT_SIZE); // 扩容
    
    // 创建新map
    map_pointer new_map = map_allocator::allocate(new_map_size);
    for(size_type i = 0 ; i < new_map_size ; i++) {
        *(new_map + i) = nullptr;
    }

    const size_type old_buffer = finish_.node - start_.node + 1;    // 旧buffer的数量
    const size_type new_buffer = old_buffer + need;                 // 新buffer的数量

    // 让新开辟的map指向原来的buffer，并且开辟新的buffer内存
    map_pointer begin = new_map + (new_map_size - new_buffer) / 2;     // 往中间放
    map_pointer mid = begin + need;                              // [begin, mid-1] 是新的buffer
    map_pointer end = mid + old_buffer;                                // [mid, end - 1] 是旧的buffer

    create_nodes(begin, mid - 1);                                      // 创建新的buffer
    for(map_pointer cur1 = mid, cur2 = start_.node ; cur1 != end ; ++cur1, ++cur2) {
        *cur1 = *cur2;                                                 // 将原来的buffer搬到新的来
    }

    // 回收原来的map内存和更新指针
    map_allocator::deallocate(map_, map_size_);
    map_ = new_map;
    map_size_ = new_map_size;
    // iterator 的first cur last 指针都没有失效，唯一失效的是map指针。也可以这样重新构造
    start_ = iterator(*mid + (start_.cur - start_.first), mid);
    finish_ = iterator(*(end - 1) + (finish_.cur - finish_.first), end - 1); 
}

template <class T>
void deque<T>::reallocate_map_at_back(size_type need) {
    const size_type new_map_size = mystl::max((map_size_ << 1), map_size_ + need + DEQUE_MAP_INIT_SIZE);
    
    map_pointer new_map = map_allocator::allocate(new_map_size);
    for(size_type i = 0 ; i < new_map_size ; i++) {
        *(new_map + i) = nullptr;
    }

    const size_type old_buffer = finish_.node - start_.node + 1;
    const size_type new_buffer = old_buffer + need;

    // 调整新的开始指针
    map_pointer begin = new_map + ((new_map_size - new_buffer) / 2);
    map_pointer mid = begin + old_buffer;                           // [begin, mid-1] 是旧的buffer
    map_pointer end = mid + need;                                   // [mid, end-1]   是新的buffer

    // 将原来的buffer搬到新的map指针中
    for(map_pointer cur1 = begin, cur2 = start_.node ; cur1 != mid ; ++cur1, ++ cur2) {
        *cur1 = *cur2;
    }
    create_nodes(mid, end - 1);

    // 回收旧map内存，更新新的deque指针
    map_allocator::deallocate(map_, map_size_);
    map_ = new_map;
    map_size_ = new_map_size;
    start_ = iterator(*begin + (start_.cur - start_.first), begin);
    finish_ = iterator(*(mid - 1) + (finish_.cur - finish_.first), mid - 1);    // 这里光扩了内存，并没有构造对象 
}


// erase
template <class T>
typename deque<T>::iterator 
deque<T>::erase(iterator pos) {
    iterator next = pos;
    ++next;
    const size_type elems_before = pos - start_;
    if(elems_before < (size() / 2)) {
        // 移动前面的元素
        mystl::copy_backward(start_, pos, next);
        pop_front();
    }else {
        mystl::copy(next, finish_, pos);
        pop_back();
    }
    return start_ + elems_before;
}

// 删除[first, last)
template <class T>
typename deque<T>::iterator 
deque<T>::erase(iterator first, iterator last) {
    if(first == start_ && last == finish_) {
        clear();
        return finish_;
    }else {
        const size_type len = last - first;     // 删除元素的长度
        const size_type elems_before = first - start_;
        if(elems_before < (size() - len) / 2) {
            // 移动前面的元素
            mystl::copy_backward(start_, first, last);
            iterator new_start = start_ + len;
            mystl::destroy(start_, new_start);
            start_ = new_start;
        }else {
            mystl::copy(last, finish_, first);
            iterator new_finish = finish_ - len;
            mystl::destroy(new_finish, finish_);
            finish_ = new_finish;
        }
        return start_ + elems_before;
    }
}


// push_front / push_back

template <class T>
void deque<T>::push_front() {
    push_front(value_type());
}

template <class T>  
void deque<T>::push_front(const value_type& value) {
    if(start_.cur != start_.first) {
        mystl::construct(start_.cur - 1, value);
        --start_.cur;
    }else {
        require_capacity(1, true);  // 保证头部空间足够
        --start_;
        mystl::construct(start_.cur, value);
    }
}

template <class T>   
void deque<T>::push_back() {
    push_back(value_type());
}


// DEBUG : 在push_back的情况下只有一个buffer，原因是在下面不足空间的时候进行了 ++finish.cur，而不能移动到下一个buffer
template <class T>
void deque<T>::push_back(const value_type& value) {
    // back的话 最后剩一个就算满
    if(finish_.cur != finish_.last - 1) {
        mystl::construct(finish_.cur, value);
        ++finish_.cur;
    }else{
        require_capacity(1, false);
        mystl::construct(finish_.cur, value);
        ++finish_;  // 注意不能++finish.cur
    }
}


// pop_front / pop_back
template <class T>
void deque<T>::pop_front() {
    MYSTL_DEBUG(!empty());
    if(start_.cur != start_.last - 1) {
        // cur不是最后一个缓冲区元素，则不用释放buffer
        mystl::destroy(start_.cur);
        ++start_.cur;
    }else{
        mystl::destroy(start_.cur);
        ++start_;
        destroy_nodes(start_.node - 1, start_.node - 1);    // 销毁这个node
    }
}

template <class T>
void deque<T>::pop_back() {
    MYSTL_DEBUG(!empty());
    if(finish_.cur != finish_.first) {
        --finish_.cur;
        mystl::destroy(finish_.cur);
    }else {
        // 也就是刚好空出一个buffer的情况，这个时候再出队尾就要释放了
        --finish_;
        mystl::destroy(finish_.cur);
        destroy_nodes(finish_.node + 1, finish_.node + 1);
    }
}



// 重载比较运算符
template <class T>
bool operator==(const deque<T>& lhs, const deque<T> rhs) {
    return lhs.size() == rhs.size() &&
        mystl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T>
bool operator!=(const deque<T>& lhs, const deque<T> rhs) {
    return !(lhs == rhs);
}

template <class T>
bool operator<(const deque<T>& lhs, const deque<T> rhs) {
    return mystl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <class T>
bool operator>=(const deque<T>& lhs, const deque<T> rhs) {
    return !(lhs < rhs);
}

template <class T>
bool operator>(const deque<T>& lhs, const deque<T> rhs) {
    return rhs < lhs;
}

template <class T>
bool operator<=(const deque<T>& lhs, const deque<T> rhs) {
    return !(lhs > rhs);
}

}

#endif