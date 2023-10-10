#ifndef __ALGOBASE_H__
#define __ALGOBASE_H__

#include <string.h>
#include <stddef.h>
#include <iostream>

#include "iterator.h"
#include "type_traits.h"
#include "util.h"

// 包含stl的基本算法
namespace mystl{

template <class T>
const T& max(const T& lhs, const T& rhs)
{
  return lhs < rhs ? rhs : lhs;
}

// 重载版本使用函数对象 comp 代替比较操作
template <class T, class Compare>
const T& max(const T& lhs, const T& rhs, Compare comp)
{
  return comp(lhs, rhs) ? rhs : lhs;
}

template <class T>
const T& min(const T& lhs, const T& rhs)
{
  return lhs < rhs ? lhs : rhs;
}

// 重载版本使用函数对象 comp 代替比较操作
template <class T, class Compare>
const T& min(const T& lhs, const T& rhs, Compare comp)
{
  return comp(lhs, rhs) ? lhs : rhs;
}


//----------------------------------------copy-------------------------------------------------------

// 迭代器的分支
template <class InputIterator, class ForwardIterator, class Distance>
ForwardIterator copy_d(InputIterator first, InputIterator last, ForwardIterator result, Distance*) {
    for(Distance n = last - first ; n > 0 ; --n, ++first, ++result) {
        *result = *first;
    } 
    return result;
}

template <class InputIterator, class ForwardIterator>
ForwardIterator copy_aux(InputIterator first, InputIterator last, ForwardIterator result, input_iterator_tag) {
    for(; first != last ; ++first, ++result) {
        *result = *first;
    }
    return result;
}

template <class InputIterator, class ForwardIterator>
ForwardIterator copy_aux(InputIterator first, InputIterator last, ForwardIterator result, random_access_iterator_tag) {
    return copy_d(first, last, result, difference_type(first));
}

template<class T>
T* copy_t(const T* first, const T* last, T* result, true_type) {
    memmove(result, first, sizeof(T) * (last - first));
    return result + (last - first);
}

template<class T>
T* copy_t(const T* first, const T* last, T* result, false_type) {
    return copy_d(first, last, result, (ptrdiff_t*)0); //random_tag
}

// 裸指针指针偏特化
template<class T>
T* copy_dispatch(const T* first, const T* last, T* result) {
    typedef typename type_traits<T>::has_trivial_assignment_operator Is_trivial;
    return copy_t(first, last, result, Is_trivial());
}

template<class T>
T* copy_dispatch(T* first, T* last, T* result) {
    typedef typename type_traits<T>::has_trivial_assignment_operator Is_trivial;
    return copy_t(first, last, result, Is_trivial());
}

// 迭代器的
template <class InputIterator, class ForwardIterator>
ForwardIterator copy_dispatch(InputIterator first, InputIterator last, ForwardIterator result) {
    return copy_aux(first, last, result, iterator_category(first));
}

char* copy(const char* first, const char* last, char* result) {
    memmove(result, first, last - first);
    return result + (last - first);
}

template <class InputIterator, class ForwardIterator>
ForwardIterator copy(InputIterator first, InputIterator last, ForwardIterator result) {
    return copy_dispatch(first, last, result);
}

//----------------------------------------copy end-----------------------------------------------------

//----------------------------------------copy_backward-------------------------------------------------------

// 将[first, last) 区间的元素拷贝到[result - (last - first), result)内
// 也就是向后拷贝，末尾为result(开区间)
template <class BidirectionalIterator1, class BidirectionalIterator2, class Distance>
BidirectionalIterator2 copy_backward_d(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result, Distance*) {
    for(Distance n = last - first ; n > 0 ; --n) {
        *--result = *--last;
    }
    return result;
}

template <class BidirectionalIterator1, class BidirectionalIterator2>
BidirectionalIterator2 copy_backward_aux(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result, bidirectional_iterator_tag) {
    while(first != last) {
        *--result = *--last; //从后往前赋值 
    }
    return result;
}

template <class BidirectionalIterator1, class BidirectionalIterator2>
BidirectionalIterator2 copy_backward_aux(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result, random_access_iterator_tag) {
    return copy_backward_d(first, last, result, difference_type(first));
}


// 不重要直接内存拷贝
template <class T>
T* copy_backward_t(const T* first, const T* last, T* result, true_type) {
    const size_t n = last - first;
    if(n != 0) {
        result -= n;
        memmove(result, first, n * sizeof(T));
    }
    return result; //返回拷贝后的首地址
}

// 重要调用用n判断的，copy_backward_d
template <class T>
T* copy_backward_t(const T* first, const T* last, T* result, false_type) {
    return copy_backward_d(first, last, result, (ptrdiff_t*)0);
}

template <class T>
T* copy_backward_dispatch(T* first, T* last, T* result) {
    typedef typename type_traits<T>::has_trivial_assignment_operator Is_trivial;
    return copy_backward_t(first, last, result, Is_trivial());
}

template <class T>
T* copy_backward_dispatch(const T* first, const T* last, T* result) {
    typedef typename type_traits<T>::has_trivial_assignment_operator Is_trivial;
    return copy_backward_t(first, last, result, Is_trivial());
}

template <class BidirectionalIterator1, class BidirectionalIterator2>
BidirectionalIterator2 copy_backward_dispatch(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result) {
    return copy_backward_aux(first, last, result, iterator_category(first));
}


template <class BidirectionalIterator1, class BidirectionalIterator2>
BidirectionalIterator2 copy_backward(BidirectionalIterator1 first, BidirectionalIterator1 last, BidirectionalIterator2 result) {
    return copy_backward_dispatch(first, last, result);
}


//----------------------------------------copy_backward end-----------------------------------------------------

//----------------------------------------fill_n---------------------------------------------------------
template <class ForwardIterator, class Size, class Tp>
ForwardIterator fill_n_aux(ForwardIterator first, Size n, const Tp& value) {
    for(; n > 0 ; --n, ++first) {
        *first = value;
    }
    return first;
}

// 如果可以的话，实现一个ont-byte特化，直接用memset

template <class ForwardIterator, class Size, class Tp>
ForwardIterator fill_n(ForwardIterator first, Size n, const Tp& value) {
    return fill_n_aux(first, n, value);
}
//----------------------------------------fill_n end-----------------------------------------------------


//----------------------------------------fill---------------------------------------------------------
template <class ForwardIterator, class Tp>
void fill_aux(ForwardIterator first, ForwardIterator last, const Tp& value, input_iterator_tag) {
    for(; first != last ; ++first) {
        *first = value; 
    }
}

template <class ForwardIterator, class Tp>
void fill_aux(ForwardIterator first, ForwardIterator last, const Tp& value, random_access_iterator_tag) {
    auto n = last - first;
    for(; n > 0 ; --n, ++first) {
        *first = value;
    }
}

template <class ForwardIterator, class Tp>
void fill(ForwardIterator first, ForwardIterator last, const Tp& value) {
    fill_aux(first, last, value, iterator_category(first));
}

//----------------------------------------fill end-----------------------------------------------------



//----------------------------------------move---------------------------------------------------------
template <class InputIterator, class OutputIterator, class Distance>
OutputIterator move_d(InputIterator first, InputIterator last, OutputIterator result, Distance*) {
    for(Distance n = last - first ; n > 0 ; --n, ++first, ++result) {
        *result = mystl::move(*first);
    } 
    return result;
}

template <class InputIterator, class OutputIterator, class Distance>
OutputIterator move_aux(InputIterator first, InputIterator last, OutputIterator result, input_iterator_tag) {
    for(; first != last ; ++first, ++result) {
        *result = mystl::move(*first);
    }
    return result;
}

template <class InputIterator, class OutputIterator, class Distance>
OutputIterator move_aux(InputIterator first, InputIterator last, OutputIterator result, random_access_iterator_tag) {
    return move_d(first, last, result, difference_type(first));
}

template <class T>
T* move_t(const T* first, const T* last, T* result, true_type) {
    memmove(result, first, sizeof(T) * (last - first));
    return result + (last - first);
}

template <class T>
T* move_t(const T* first, const T* last, T* result, false_type) {
    return move_d(first, last, result, ptrdiff_t(0)); //trivial
}


template <class T>
T* move_dispatch(const T* first, const T* last, T* result) {
    typedef typename type_traits<T>::has_trivial_copy_constructor is_trivial;
    return move_t(first, last, result, is_trivial());
}

template <class T>
T* move_dispatch(T* first, T* last, T* result) {
    typedef typename type_traits<T>::has_trivial_copy_constructor is_trivial;
    return move_t(first, last, result, is_trivial());
}

template <class InputIterator, class OutputIterator>
OutputIterator move_dispatch(InputIterator first, InputIterator last, OutputIterator result) {
    return move_aux(first, last, result, iterator_category(first));
}

char* move(const char* first, const char* last, char* result) {
    memmove(result, first, last - first);
    return result + (last - first);
}

char* move(char* first,char* last, char* result) {
    memmove(result, first, last - first);
    return result + (last - first);
}

template <class InputIterator, class OutputIterator>
OutputIterator move(InputIterator first, InputIterator last, OutputIterator result) {
    return move_dispatch(first, last, result);
}

//----------------------------------------move end---------------------------------------------------------


// 比较区间[first1, last1) 与同等区间[first1, last2)是否相等，不判断last2之后的
template <class InputIterator1, class InputIterator2>
bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2) {
    for(; first1 != last1 ; ++first1, ++first2) {
        if((*first1 != *first2)) {
            return false;
        }
    }
    return true;
}

// 重载具有断言函数的(判断二者是否相等的)
template <class InputIterator1, class InputIterator2, class BiPredicate>
bool equal(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, BiPredicate pred) {
    for(; first1 != last1 ; ++first1, ++first2) {
        if(!pred(*first1, *first2)) {
            return false;
        }
    }
    return true;
}

// 字典排序
template <class InputIterator1, class InputIterator2>
bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2) {
    for(; first1 != last1 && first2 != last2 ; ++first1, ++first2) {
        if(*first1 < *first2) {
            return true;
        } else if(*first2 < *first1) {
            return false;
        }
        // 否则继续比较
    }
    return first1 == last1 && first2 != last2; //for循环没有比较完的话，区间1的长度一定要比区间2的小，才能返回true
}

template <class InputIterator1, class InputIterator2, class Compare>
bool lexicographical_compare(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, InputIterator2 last2, Compare comp) {
    for(; first1 != last1 && first2 != last2 ; ++first1, ++first2) {
        if(comp(*first1, *first2)) {
            return true;
        } else if(comp(*first2, *first1)) {
            return false;
        }
        // 否则继续比较
    }
    return first1 == last1 && first2 != last2; //for循环没有比较完的话，区间1的长度一定要比区间2的小，才能返回true
}


}

#endif