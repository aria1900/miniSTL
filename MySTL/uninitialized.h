#ifndef __UNINITIALIZED_H__
#define __UNINITIALIZED_H__

#include <string.h>

#include "algobase.h"
#include "iterator.h"
#include "type_traits.h"
#include "construct.h"
#include "util.h"

// 这个头文件定义了关于区间初始化的一些函数.uninitialized_copy,uninitialized_fill,uninitialized_copy_n,uninitialized_fill_n
// 为什么初始化的copy不用全局copy呢，全局copy实际上执行了copy_ctor or operator=,而初始化的copy是直接在内存上构造对象，要快的多
// 初始化的fill也同理，是直接调用构造函数，而不是copy_ctor
// 但如果不是POD，那么就可以直接采用全局的函数，有更加细分的分支，有关于trivial operator=的信息，T*的分支上

namespace mystl{

template <class InputIterator, class ForwardIterator>
ForwardIterator uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, true_type) {
    return mystl::copy(first, last, result); //当是POD时，直接调用上层的copy，效率更高
}

// 非POD，则需要逐个构造对象
template <class InputIterator, class ForwardIterator>
ForwardIterator uninitialized_copy_aux(InputIterator first, InputIterator last, ForwardIterator result, false_type) {
    ForwardIterator cur = result;
    try
    {
        for(; first != last ; ++cur, ++first) {
            mystl::construct(&*cur, *first);
        }
        return cur;
    }
    catch(...)
    {   
        // 若异常，则需要全部析构
        std::cerr << "uninitialized_copy error!" << std::endl;
        mystl::destroy(result, cur);
        throw;
    }
}

template <class InputIterator, class ForwardIterator, class Tp>
ForwardIterator uninitialized_copy_dispatch(InputIterator first, InputIterator last, ForwardIterator result, Tp*) {
    typedef typename type_traits<Tp>::is_POD_type is_POD;
    return uninitialized_copy_aux(first, last, result, is_POD());
}

// char*的特化版本
char* uninitialized_copy(const char* first, const char* last, char* resullt) {
    memmove(resullt, first, last - first);
    return resullt + (last - first);
}

template <class InputIterator, class ForwardIterator>
ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result) {
    return uninitialized_copy_dispatch(first, last, result, value_type(first));
}

template <class ForwardIterator, class Tp>
void uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const Tp& value, true_type) {
    mystl::fill(first, last, value);
}

template <class ForwardIterator, class Tp>
void uninitialized_fill_aux(ForwardIterator first, ForwardIterator last, const Tp& value, false_type) {
    ForwardIterator cur = first;
    try {
        for(; cur != last ; ++cur) {
            mystl::construct(&*cur, value);
        }
    }catch(...) {
        std::cerr << "uninitialized_fill error!" << std::endl;
        mystl::destroy(first, cur);
    }
}

template <class ForwardIterator, class Tp>
void uninitialized_fill(ForwardIterator first, ForwardIterator last, const Tp& value) {
    typedef typename iterator_traits<ForwardIterator>::value_type Value_type;
    typedef typename type_traits<Value_type>::is_POD_type is_POD;
    uninitialized_fill_aux(first, last, value, is_POD());
}

template <class ForwardIterator, class Size, class Tp>
ForwardIterator uninitialized_fill_n_aux(ForwardIterator first, Size n, const Tp& value, true_type) {
   return fill_n(first, n, value);
}

template <class ForwardIterator, class Size, class Tp>
ForwardIterator uninitialized_fill_n_aux(ForwardIterator first, Size n, const Tp& value, false_type) {
    ForwardIterator cur = first;
    try{
        for(; n > 0 ; n--, ++cur) {
            mystl::construct(&*cur, value);
        }
        return cur;
    }catch(...) {
        std::cerr << "uninitialized_fill_n error!" << std::endl;
        mystl::destroy(first, cur);
    }
}

template <class ForwardIterator, class Size,class Tp>
ForwardIterator uninitialized_fill_n(ForwardIterator first, Size n, const Tp& value) {
    typedef typename iterator_traits<ForwardIterator>::value_type Value_type;
    typedef typename type_traits<Value_type>::is_POD_type is_POD;
    return uninitialized_fill_n_aux(first, n, value, is_POD());
}


template <class InputIterator, class ForwardIterator>
ForwardIterator uninitialized_move_aux(InputIterator first, InputIterator last, ForwardIterator result, true_type) {
    return mystl::move(first, last, result);
}

template <class InputIterator, class ForwardIterator>
ForwardIterator uninitialized_move_aux(InputIterator first, InputIterator last, ForwardIterator result, false_type) {
    ForwardIterator cur = result;
    try{
        for(; first != last ; ++cur, ++first) {
            mystl::construct(&*cur, mystl::move(*first)); //将其变成一个右值，用完就析构
        }
        return cur;
    }catch(...) {
        mystl::destroy(result, cur);
    }
}

template <class InputIterator, class ForwardIterator>
ForwardIterator uninitialized_move(InputIterator first, InputIterator last, ForwardIterator result) {
    typedef typename type_traits<typename iterator_traits<InputIterator>::value_type>::is_POD_type is_POD;
    return uninitialized_move_aux(first, last, result, is_POD());
}
}

#endif