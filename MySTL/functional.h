#ifndef __FUNCTIONAL_H__
#define __FUNCTIONAL_H__

// 这个头文件定义了六大组件之一的Functors

namespace mystl {

// 定义一元函数的模板，所有的Functors必须继承该类型，否则无法被适配
// 因为Adapter要提取器内置的型别

template <class Arg, class Result>
struct  unary_function {
    typedef Arg argument_type;
    typedef Result result_type;
};

template <class Arg1, class Arg2, class Result>
struct binary_function {
    typedef Arg1 first_argument_type;
    typedef Arg2 second_argument_type;
    typedef Result result_type;
};

// operator<
template <class T>
struct less : public binary_function<T, T, bool> {
    bool operator()(const T& lhs, const T& rhs) const {
        return lhs < rhs;
    }
};

template <class T>
struct greater : public binary_function<T, T, bool> {
    bool operator()(const T& lhs, const T& rhs) const {
        return lhs > rhs;
    }
};

// <=
template <class T>
struct less_equal : public binary_function<T, T, bool> {
    bool operator()(const T& lhs, const T& rhs) const {
        return lhs <= rhs;
    }
};

// >=
template <class T>
struct greater_equal : public binary_function<T, T, bool> {
    bool operator()(const T& lhs, const T& rhs) const {
        return lhs >= rhs;
    }
};

// 函数对象：等于 ==
template <class T>
struct equal_to : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x == y; }
};

// !=
template <class T>
struct not_equal_to : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return x != y; }
};


// 证同函数：返回传入参数本身
template <class T>
struct identity : public unary_function<T, T> {
    const T& operator()(const T& x) const { return x; }
};

// 选择pair的第一个或第二个参数
template <class Pair>
struct select1st : public unary_function<Pair, typename Pair::first_type> {
    const typename Pair::first_type& operator()(const Pair& x) const { return x.first; }
};

template <class Pair>
struct select2nd : public unary_function<Pair, typename Pair::second_type> {
    const typename Pair::second_type& operator()(const Pair& x) const { return x.second; }
};

// 投射函数：返回第一参数
template <class Arg1, class Arg2>
struct project1st : public binary_function<Arg1, Arg2, Arg1> {
    Arg1 operator()(const Arg1& x, const Arg2& y) const { return x; }
};

template <class Arg1, class Arg2>
struct project2nd : public binary_function<Arg1, Arg2, Arg2> {
    Arg2 operator()(const Arg1& x, const Arg2& y) const { return y; }
};


// 哈希函数对象
// 对于大部分基本类型，就返回类型本身
template <class Key>
struct hash {
    size_t operator()(const Key& k) const {
        return k;
    }
};

// 指针偏特化
template <class T>
struct hash<T*> {
    size_t operator()(T* ptr) const {
        return reinterpret_cast<size_t>(ptr);
    }
};

template <class T>
struct hash<const T*> {
    size_t operator()(const T* ptr) const {
        return reinterpret_cast<size_t>(ptr);
    }
};


// 函数对像 : 加法
template <class T>
struct plus : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const {
        return x + y;
    }
};

// 函数对象 : 减法
template <class T>
struct minus : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const {
        return x - y;
    }
};

// 函数对象：乘法
template <class T>
struct multiplies : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { 
        return x * y; 
    }
};

// 函数对象：除法
template <class T>
struct divides : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const { 
        return x / y; 
    }
};

// 函数对象 : 模运算
template <class T>
struct modulus : public binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const {
        return x % y;
    }
};

// 函数对象 : 否定
template <class T>
struct negate : public unary_function<T, T> {
    T operator()(const T& x) const { 
        return -x;
    }
};

// 单位圆 : function, not functor
template <class T>
T identity_element(plus<T>) {
    return T(0);    // x + 0 = x 
}

template <class T>
T identity_element(multiplies<T>) {
    return T(1);    // x * 1 = x
}


// 逻辑运算 && || !
template <class T>
struct logical_and : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const {
        return x && y;
    }
};

template <class T>
struct logical_or : public binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const {
        return x || y;
    }
};

template <class T>
struct logical_not : public unary_function<T, bool> {
    bool operator()(const T& x) const {
        return !x;
    }
};



// --------------------function adapter--------------------

// binder1st : 绑定二元函数的第一参数，让他变成一元函数
// 注意Operation必须可配接，继承unary_funcion等，一般用于functional类
template <class Operation>
class binder1st : public unary_function<typename Operation::second_argument_type,
                                        typename Operation::result_type> {
protected:
    Operation op;
    typename Operation::first_argument_type value;  // 第一参数

public:
    // 构造函数，将该函数functor保存，并且将第一参数保存，以至于使用的时候，可以及时转调用
    binder1st(const Operation& x, const typename Operation::first_argument_type& y) : op(x), value(y) { }

    typename Operation::result_type
    operator()(const typename Operation::second_argument_type& x) const {
        return op(value, x);    // 对于这个对象，进行调用，也就是将保存的值再次转调用
    }
};

// 辅助函数用于创建一个binder1st对象，并返回
template <class Operation, class T>
inline binder1st<Operation>     // 返回binder1st对象
bind1st(const Operation& fn, const T& x) {
    typedef typename Operation::first_argument_type Arg1;
    return binder1st<Operation>(fn, Arg1(x));
}

// binder2nd : 绑定二元函数的第二参数，让他变成一元函数
template <class Operation>
class binder2nd : public unary_function<typename Operation::first_argument_type,
                                        typename Operation::result_type> {
protected:
    Operation op;
    typename Operation::second_argument_type value;  // 第一参数

public:
    // 构造函数，将该函数functor保存，并且将第一参数保存，以至于使用的时候，可以及时转调用
    binder2nd(const Operation& x, const typename Operation::second_argument_type& y) : op(x), value(y) { }

    typename Operation::result_type
    operator()(const typename Operation::first_argument_type& x) const {
        return op(x, value);    // 对于这个对象，进行调用，也就是将保存的值再次转调用
    }
};

// 辅助函数用于创建一个binder1st对象，并返回
template <class Operation, class T>
inline binder2nd<Operation>     // 返回binder1st对象
bind2nd(const Operation& fn, const T& x) {
    typedef typename Operation::second_argument_type Arg2;
    return binder2nd<Operation>(fn, Arg2(x));
}

}

#endif