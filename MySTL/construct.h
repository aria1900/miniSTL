#ifndef __CONSTRUCT_H__
#define __CONSTRUCT_H__

#include "type_traits.h"
#include "iterator.h"

#include <new>

// 这个头文件定义的是对象的构造和析构相关的函数

namespace mystl{

// 调用全局的带参的new，来进行指定地址的对象构造
template <class Tp>
inline void construct(Tp* ptr) {
    ::new((void*)ptr) Tp();
}

template <class Tp1, class Tp2>
inline void construct(Tp1* ptr, const Tp2& value){
    ::new((void*)ptr) Tp1(value); // copy和普通ctor皆可
}

// 显示的调用析构函数，完成对象的析构
// 这里通过type_traits，分两个支线，一个调用dtor，另一个则不用调用，增加效率

template <class Tp>
inline void destroy_one(Tp* ptr, true_type) {
    
}

template <class Tp>
inline void destroy_one(Tp* ptr, false_type) {
    ptr->~Tp();
}

template <class Tp>
inline void destroy(Tp* ptr) {
    typedef typename type_traits<Tp>::has_trivial_destructor trivial_destructor;
    destroy_one(ptr, trivial_destructor());
}

// 这里的特化应该是 是否有trivial的dtor
// 如果non-trivial 则一个个调用析构
template <class ForwardIterator>
inline void destroy_aux(ForwardIterator first, ForwardIterator last, false_type) {
    //std::cout << "non-trivial dtor, ~Tp()" << std::endl;
    for(; first != last ; ++first) {
        destroy(&*first); //获取裸指针
    }
}

// 否则不用管
template <class ForwardIterator>
inline void destroy_aux(ForwardIterator first, ForwardIterator last, true_type) {
    //std::cout << "trivial dtor, do nothing" << std::endl;    
}

// 这个函数获取迭代器指向对象的类型
template <class ForwardIterator, class Tp>
inline void destroy_dispatch(ForwardIterator first, ForwardIterator last, Tp*) {
    typedef typename type_traits<Tp>::has_trivial_destructor trivial_destructor;
    destroy_aux(first, last, trivial_destructor()); //这里传入true或者false
}

template <class ForwardIterator>
inline void destroy(ForwardIterator first, ForwardIterator last) {
    destroy_dispatch(first, last, value_type(first));
}

} // namespace mystl

#endif // !__CONSTRUCT_H__