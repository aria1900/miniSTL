#ifndef __TYPE_TRAITS_H__
#define __TYPE_TRAITS_H__

// 这个头文件定义了一些type_traits，是否为POD之类的

namespace mystl{

// 这里定义struct是为了模板参数的型别认定，而不是单纯的typedef一个bool
struct true_type {};
struct false_type {};

// 通用的类型，都是false_type，为了保守
// 但实际上这个类重度依赖模板特化
template <class Tp>
struct type_traits {
    // POD具体来说它是指没有使用面相对象的思想来设计的类/结构体。
    // POD的全称是Plain Old Data，Plain表明它是一个普通的类型，没有虚函数虚继承等特性；Old表明它与C兼容.

    typedef false_type has_trivial_default_constructor; // 默认构造函数
    typedef false_type has_trivial_copy_constructor;    // 拷贝构造函数
    typedef false_type has_trivial_assignment_operator; // operator=
    typedef false_type has_trivial_destructor;          // 析构函数
    typedef false_type is_POD_type;                     // 旧类型
};

// 以下基本数据类型都是trivial的
template <>
struct type_traits<bool> {
    typedef true_type has_trivial_default_constructor;
    typedef true_type has_trivial_copy_constructor;
    typedef true_type has_trivial_assignment_operator;
    typedef true_type has_trivial_destructor;
    typedef true_type is_POD_type;
};

template <>
struct type_traits<char> {
    typedef true_type has_trivial_default_constructor;
    typedef true_type has_trivial_copy_constructor;
    typedef true_type has_trivial_assignment_operator;
    typedef true_type has_trivial_destructor;
    typedef true_type is_POD_type;
};

template <>
struct type_traits<signed char> {
    typedef true_type has_trivial_default_constructor;
    typedef true_type has_trivial_copy_constructor;
    typedef true_type has_trivial_assignment_operator;
    typedef true_type has_trivial_destructor;
    typedef true_type is_POD_type;
};

template <>
struct type_traits<unsigned char> {
    typedef true_type has_trivial_default_constructor;
    typedef true_type has_trivial_copy_constructor;
    typedef true_type has_trivial_assignment_operator;
    typedef true_type has_trivial_destructor;
    typedef true_type is_POD_type;
};

template <>
struct type_traits<short> {
    typedef true_type has_trivial_default_constructor;
    typedef true_type has_trivial_copy_constructor;
    typedef true_type has_trivial_assignment_operator;
    typedef true_type has_trivial_destructor;
    typedef true_type is_POD_type;
};

template <>
struct type_traits<unsigned short> {
    typedef true_type has_trivial_default_constructor;
    typedef true_type has_trivial_copy_constructor;
    typedef true_type has_trivial_assignment_operator;
    typedef true_type has_trivial_destructor;
    typedef true_type is_POD_type;
};

template <>
struct type_traits<int> {
    typedef true_type has_trivial_default_constructor;
    typedef true_type has_trivial_copy_constructor;
    typedef true_type has_trivial_assignment_operator;
    typedef true_type has_trivial_destructor;
    typedef true_type is_POD_type;
};

template <>
struct type_traits<unsigned int> {
    typedef true_type has_trivial_default_constructor;
    typedef true_type has_trivial_copy_constructor;
    typedef true_type has_trivial_assignment_operator;
    typedef true_type has_trivial_destructor;
    typedef true_type is_POD_type;
};

template <>
struct type_traits<long> {
    typedef true_type has_trivial_default_constructor;
    typedef true_type has_trivial_copy_constructor;
    typedef true_type has_trivial_assignment_operator;
    typedef true_type has_trivial_destructor;
    typedef true_type is_POD_type;
};

template <>
struct type_traits<unsigned long> {
    typedef true_type has_trivial_default_constructor;
    typedef true_type has_trivial_copy_constructor;
    typedef true_type has_trivial_assignment_operator;
    typedef true_type has_trivial_destructor;
    typedef true_type is_POD_type;
};

template <>
struct type_traits<float> {
    typedef true_type has_trivial_default_constructor;
    typedef true_type has_trivial_copy_constructor;
    typedef true_type has_trivial_assignment_operator;
    typedef true_type has_trivial_destructor;
    typedef true_type is_POD_type;
};

template <>
struct type_traits<double> {
    typedef true_type has_trivial_default_constructor;
    typedef true_type has_trivial_copy_constructor;
    typedef true_type has_trivial_assignment_operator;
    typedef true_type has_trivial_destructor;
    typedef true_type is_POD_type;
};

template <>
struct type_traits<long double> {
    typedef true_type has_trivial_default_constructor;
    typedef true_type has_trivial_copy_constructor;
    typedef true_type has_trivial_assignment_operator;
    typedef true_type has_trivial_destructor;
    typedef true_type is_POD_type;
};

// c风格字符串
template <>
struct type_traits<char *> {
    typedef true_type has_trivial_default_constructor;
    typedef true_type has_trivial_copy_constructor;
    typedef true_type has_trivial_assignment_operator;
    typedef true_type has_trivial_destructor;
    typedef true_type is_POD_type;
};

template <>
struct type_traits<signed char*> {
    typedef true_type has_trivial_default_constructor;
    typedef true_type has_trivial_copy_constructor;
    typedef true_type has_trivial_assignment_operator;
    typedef true_type has_trivial_destructor;
    typedef true_type is_POD_type;
};

template <>
struct type_traits<unsigned char*> {
    typedef true_type has_trivial_default_constructor;
    typedef true_type has_trivial_copy_constructor;
    typedef true_type has_trivial_assignment_operator;
    typedef true_type has_trivial_destructor;
    typedef true_type is_POD_type;
};

template <>
struct type_traits<const char> {
    typedef true_type has_trivial_default_constructor;
    typedef true_type has_trivial_copy_constructor;
    typedef true_type has_trivial_assignment_operator;
    typedef true_type has_trivial_destructor;
    typedef true_type is_POD_type;
};

template <>
struct type_traits<const signed char*> {
    typedef true_type has_trivial_default_constructor;
    typedef true_type has_trivial_copy_constructor;
    typedef true_type has_trivial_assignment_operator;
    typedef true_type has_trivial_destructor;
    typedef true_type is_POD_type;
};

template <>
struct type_traits<const unsigned char*> {
    typedef true_type has_trivial_default_constructor;
    typedef true_type has_trivial_copy_constructor;
    typedef true_type has_trivial_assignment_operator;
    typedef true_type has_trivial_destructor;
    typedef true_type is_POD_type;
};

// 这个类用于识别类型是否为整数
// 默认是false，如果是整数，会走到偏特化方向
template <class T>
struct is_integral {
    typedef false_type value;
};

template <>
struct is_integral<short> {
    typedef true_type value;
};

template <>
struct is_integral<unsigned short> {
    typedef true_type value;
};


template <>
struct is_integral<int> {
    typedef true_type value;
};

template <>
struct is_integral<unsigned int> {
    typedef true_type value;
};


template <>
struct is_integral<long> {
    typedef true_type value;
};

template <>
struct is_integral<unsigned long> {
    typedef true_type value;
};

template <>
struct is_integral<long long> {
    typedef true_type value;
};

template <>
struct is_integral<unsigned long long> {
    typedef true_type value;
};

}

#endif // !__TYPE_TRAITS_H__