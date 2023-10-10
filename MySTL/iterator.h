#ifndef __ITERATOR_H__
#define __ITERATOR_H__

#include <cstddef>
#include <iostream>

// 这个头文件用于定义迭代器类型、iterator traits和一些常用的iterator adapter

namespace mystl {

// 五种迭代器类型
struct output_iterator_tag {};
struct input_iterator_tag {};
struct forward_iterator_tag : input_iterator_tag {};
struct bidirectional_iterator_tag : forward_iterator_tag {};
struct random_access_iterator_tag : bidirectional_iterator_tag {};

// iterator模板
template <class Category,
          class T,
          class Differece = ptrdiff_t,
          class Pointer = T*,
          class Reference = T&>
struct iterator {
    typedef Category iterator_category;
    typedef T value_type;
    typedef Differece difference_type;
    typedef Pointer pointer;
    typedef Reference reference;
};

// iterator traits
template <class Iterator>
struct iterator_traits {
    typedef typename Iterator::iterator_category iterator_category;
    typedef typename Iterator::value_type value_type;
    typedef typename Iterator::difference_type difference_type;
    typedef typename Iterator::pointer pointer;
    typedef typename Iterator::reference reference;
};

// 裸指针特化
template <class T>
struct iterator_traits<T*> {
    typedef random_access_iterator_tag iterator_category;
    typedef T value_type;
    typedef ptrdiff_t difference_type;
    typedef T* pointer;
    typedef T& reference;
};

template <class T>
struct iterator_traits<const T*> {
    typedef random_access_iterator_tag iterator_category;
    typedef const T value_type;
    typedef ptrdiff_t difference_type;
    typedef const T* pointer;
    typedef const T& reference;
};

// 以下为3种萃取函数，萃取迭代器的tag, value_type, 和difference_type
// 第三种有可能一般在需要迭代器的距离的时候用到，比如heap算法

template <class Iterator>
inline typename iterator_traits<Iterator>::iterator_category 
iterator_category(const Iterator&) {
    typedef typename iterator_traits<Iterator>::iterator_category Category;
    return Category();  // 返回临时对象，也就是该tag
}

template <class Iterator>
inline typename iterator_traits<Iterator>::value_type* 
value_type(const Iterator&) {
    typedef typename iterator_traits<Iterator>::value_type Value_type;
    return static_cast<Value_type*>(0);
}

template <class Iterator>
inline typename iterator_traits<Iterator>::difference_type* 
difference_type(const Iterator&) {
    typedef typename iterator_traits<Iterator>::difference_type Difference_type;
    return static_cast<Difference_type*>(0);
}

// 以下两种函数，用于计算迭代器的距离，和让迭代器前进n个距离，属于迭代器的辅助函数

template <class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance_dispatch(InputIterator first, InputIterator last, input_iterator_tag) {
    typename iterator_traits<InputIterator>::difference_type dist = 0;
    for(; first != last ; ++first){
        ++dist;
    }
    return dist;
}

template <class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type
distance_dispatch(InputIterator first, InputIterator last, random_access_iterator_tag) {
    return last - first;
}

template <class InputIterator>
inline typename iterator_traits<InputIterator>::difference_type 
distance(InputIterator first, InputIterator last) {
    return distance_dispatch(first, last, iterator_category(first));  // 类型识别
}

template <class InputIterator, class Distance>
inline void
advance_dispatch(InputIterator& first, Distance n, input_iterator_tag) {
    while(n --){
        ++first;
    }
}

// 双向迭代器有可能出现 n < 0 的情况
template <class InputIterator, class Distance>
inline void
advance_dispatch(InputIterator& first, Distance n, bidirectional_iterator_tag) {
    if(n >= 0){
        while(n --){
            ++first;
        }
    }else{
        while(n ++){
            --first;
        }
    }
}

template <class InputIterator, class Distance>
inline void
advance_dispatch(InputIterator& first, Distance n, random_access_iterator_tag) {
    first += n;
}

template <class InputIterator, class Distance>
inline void
advance(InputIterator& first, Distance n) {
    advance_dispatch(first, n, iterator_category(first));
}

// 以下留着写 iterator adapter, 如insert_iterator、stream_iterator和reverse_iterator等

//---------------------------reverse_iterator-------------------------------
template <class Iterator>
class reverse_iterator {
public:
    // 型别定义，reverse的各种型别都是正向迭代器的
    typedef typename iterator_traits<Iterator>::iterator_category iterator_category;
    typedef typename iterator_traits<Iterator>::value_type value_type;
    typedef typename iterator_traits<Iterator>::difference_type difference_type;
    typedef typename iterator_traits<Iterator>::pointer pointer;
    typedef typename iterator_traits<Iterator>::reference reference;

    typedef Iterator iterator_type;             //正向迭代器类型
    typedef reverse_iterator<Iterator> self;    //反向迭代器自己

private:
    iterator_type current;
public:
    // 构造 析构函数
    reverse_iterator() {}
    explicit reverse_iterator(iterator_type x) : current(x) {}
    reverse_iterator(const self& x) : current(x.current) {}

    iterator_type base() const { return current; } //取出对应的正向迭代器

public:
    reference operator*() const {
        iterator_type tmp = current;
        return *--tmp;
    }

    pointer operator->() const { return &(operator*()); }

    // 指针操作
    self& operator++() {
        --current;
        return *this;
    }

    self operator++(int) {
        self tmp = *this;
        --current;
        return tmp;
    }

    self& operator--() {
        ++current;
        return *this;
    }

    self operator--(int) {
        self tmp = *this;
        ++current;
        return tmp;
    }

    self operator+(difference_type n) const {
        return self(current - n);
    }

    self operator+=(difference_type n) {
        current -= n;
        return *this;
    }

    self operator-(difference_type n) const {
        return self(current + n);
    }

    self operator-=(difference_type n) {
        current += n;
        return *this;
    }

    reference operator[](difference_type n) const {
        return *(*this + n);
    }
};

// 重载两个反向迭代器的operator-
template <class Iterator>
typename reverse_iterator<Iterator>::difference_type
operator-(const reverse_iterator<Iterator>& lhs,
          const reverse_iterator<Iterator>& rhs) { 
    return rhs.base() - lhs.base(); //a - b 反过来了 b - a
}

// 重载比较运算符== != < > <= >=  全局的运算符
template <class Iterator>
bool operator==(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs) {
    return lhs.base() == rhs.base();
}

template <class Iterator>
bool operator!=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs) {
    return !(lhs == rhs);
}

template <class Iterator>
bool operator<(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs) {
    return rhs.base() < lhs.base(); //反过来了
}

template <class Iterator>
bool operator>(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs) {
    return rhs.base() > lhs.base(); //反过来了
}

template <class Iterator>
bool operator<=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs) {
    return !(rhs < lhs); // l <= r 否定之后 l > r
}

template <class Iterator>
bool operator>=(const reverse_iterator<Iterator>& lhs,
                const reverse_iterator<Iterator>& rhs) {
    return !(rhs > lhs);
}

//---------------------------reverse_iterator end-------------------------------


//---------------------------insert_iterator-------------------------------
// 实现了3种插入迭代器，分别为前插尾插插入，分别转调用与之关联容器的 push_back / push_front / insert


// *result = *first; ++result, ++first    这种场景
template <class Container>  // 模板参数为容器
class back_insert_iterator {
protected:
    Container* con_;            // 底层容器
public:
    // 迭代器的五种typedef，其他的都是void
    typedef     output_iterator_tag     iterator_category;
    typedef     void                    value_type;
    typedef     void                    difference_type;
    typedef     void                    pointer;
    typedef     void                    reference;

    // 不允许隐式转化，传指针就好
    explicit back_insert_iterator(Container& x) : con_(&x) { }

    // 将赋值操作转化为插入
    back_insert_iterator<Container>&
    operator=(const typename Container::value_type& value) {
        con_->push_back(value);
        return *this;
    }

    // 关闭功能，只需要operator=
    back_insert_iterator<Container>& operator*() { return *this; }
    back_insert_iterator<Container>& operator->() { return *this; }
    back_insert_iterator<Container>& operator++() { return *this; }
    back_insert_iterator<Container>& operator++(int) { return *this; }
};

// 使用方法 如果想将[first, last) 内容插入到容器尾部，除了传入end()，也可以用后插配接器，但前提是容器提供了push_back
// vector<int> con;
// copy(first, last, back_inserter(con));
template <class Container>
back_insert_iterator<Container> back_inserter(Container& x) {
    return back_insert_iterator<Container>(x);
}


// 将赋值转化为前插
template <class Container>  // 模板参数为容器
class front_insert_iterator {
protected:
    Container* con_;            // 底层容器
public:
    // 迭代器的五种typedef，其他的都是void
    typedef     output_iterator_tag     iterator_category;
    typedef     void                    value_type;
    typedef     void                    difference_type;
    typedef     void                    pointer;
    typedef     void                    reference;

    // 不允许隐式转化，传指针就好
    explicit front_insert_iterator(Container& x) : con_(&x) { }

    // 将赋值操作转化为插入
    front_insert_iterator<Container>&
    operator=(const typename Container::value_type& value) {
        con_->push_front(value);
        return *this;
    }

    // 关闭功能，只需要operator=
    front_insert_iterator<Container>& operator*() { return *this; }
    front_insert_iterator<Container>& operator->() { return *this; }
    front_insert_iterator<Container>& operator++() { return *this; }
    front_insert_iterator<Container>& operator++(int) { return *this; }
};

template <class Container>
front_insert_iterator<Container> front_inserter(Container& x) {
    return front_insert_iterator<Container>(x);
}


template <class Container>  // 模板参数为容器
class insert_iterator {
protected:
    Container* con_;                        // 底层容器
    typename Container::iterator    it_;     // 插入位置的迭代器
public:
    // 迭代器的五种typedef，其他的都是void
    typedef     output_iterator_tag     iterator_category;
    typedef     void                    value_type;
    typedef     void                    difference_type;
    typedef     void                    pointer;
    typedef     void                    reference;

    // 不允许隐式转化，传指针就好
    explicit insert_iterator(Container& x, typename Container::iterator i) : con_(&x), it_(i) { }

    // 将赋值操作转化为插入
    insert_iterator<Container>&
    operator=(const typename Container::value_type& value) {
        it_ = con_->insert(it_, value);        // 在it位置插入value
        ++it_;  // 移动迭代器
        return *this;
    }

    // 关闭功能，只需要operator=
    insert_iterator<Container>& operator*() { return *this; }
    insert_iterator<Container>& operator->() { return *this; }
    insert_iterator<Container>& operator++() { return *this; }
    insert_iterator<Container>& operator++(int) { return *this; }
};


// 使用方法, 用于set这种内部自己维护有序，如果用copy，肯定不能使用源码的样子，必须转接，转成一个个插入
// set<int> hash; auto it = set.begin();
// copy(first, last, inserter(hash, it));   这样就能将[first, last)的内容插入进set了
template <class Container, class Iterator>
insert_iterator<Container> inserter(Container& x, Iterator it) {
    typedef typename Container::iterator    iterator;
    return insert_iterator<Container>(x, iterator(it));
}

//---------------------------insert_iterator end-------------------------------



//---------------------------stream_iterator----------------------------------

// 输出流迭代器，传入result位置的迭代器，可以将内容输出到输出流上面去
// *result = *first;
template <class T>
class ostream_iterator {
protected:
    std::ostream* os_;
    const char* str_;        // 每次输出间隔的字符串

public:
    typedef output_iterator_tag     iterator_category;
    typedef void                    value_type;
    typedef void                    difference_type;
    typedef void                    pointer;
    typedef void                    reference;

    ostream_iterator(std::ostream& s) : os_(&s), str_(" ") { }    // 默认空格
    explicit ostream_iterator(std::ostream& s, const char* str) : os_(&s), str_(str) { }

    ostream_iterator<T>& operator=(const T& value) const {
        os_ << value << str_;
        return *this;
    }

    ostream_iterator<T>& operator*()        { return *this; }
    ostream_iterator<T>& operator++()       { return *this; }
    ostream_iterator<T>& operator++(int)    { return *this; }
};


// 输入流迭代器，可以将输入的数据，作为一个区间使用，[first, last)
// 有关++的操作会被引导调用operator>>，operator*就读取到所输入的数据了，这样一个个的读入就能作为[first, last)使用了
template <class T>
class istream_iterator{
protected:
    std::istream*    is_;            // 输入流指针
    T           value;          // 每次输入的一个数据
    bool        end_marker;     // 获取流的状态，判断是否输入结束


    // 每次read都保存一个新的value
    void read() {
        end_marker = (*is_) ? true : false;  // 如果is的状态无错误，则为true，istream作为逻辑值，是重载了表示其状态的（类型转换函数重载）
        if(end_marker) {
            *is_ >> value;      // 读取数据
        }
        end_marker = (*is_) ? true : false; // 在读取一次，因为读取完成后可能会变，若为eof，则为false
    }

public:
    typedef input_iterator_tag      iterator_category;
    typedef T                       value_type;
    typedef ptrdiff_t               difference_type;
    typedef T*                      pointer;
    typedef T&                      reference;

    // istream_iterator<T> eos; 这就能代表end()
    istream_iterator() : is_(&std::cin), end_marker(false) {}            // 默认构造就是cin的false状态，也就是迭代器的end()
    explicit istream_iterator(std::istream& is) : is_(&is) { read(); }   // 首次构造就要读取一个数据，后序的读取在++操作上

    reference   operator*() const { return value; }         // *result = *first;  作为first
    pointer     operator->() const { return &(operator*()); }

    istream_iterator<T>& operator++() {
        read();
        return *this;
    }

    istream_iterator<T> operator++(int) {
        istream_iterator<T> tmp = *this;
        read();
        return tmp;
    }

    // 为了 first != last 还需要operator==
    friend bool operator==(const istream_iterator<T>& x, const istream_iterator<T>& y);
};

template <class T>
bool operator==(const istream_iterator<T>& x, const istream_iterator<T>& y) {
    return x.end_marker == y.end_marker;    // 都为false
}

//---------------------------stream_iterator  end----------------------------------


}  // namespace mystl

#endif