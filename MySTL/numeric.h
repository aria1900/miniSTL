#ifndef __NUMERIC_H__
#define __NUMERIC_H__

#include "iterator.h"

// 包含了STL的数值算法， accumulate、adjacent_difference、inner_product、partial_sum、power、iota

namespace mystl {

// accumulate
// 1: 以初值init，对每个元素累加
// 2: 以初值init，对每个元素进行二元运算, op(init, elem)
template <class InputIterator, class T>
T accumulate(InputIterator first, InputIterator last, T init) {
    for(; first != last ; ++first) {
        init += *first;
    }
    return init;
}

template <class InputIterator, class T, class BinaryOp>
T accumulate(InputIterator first, InputIterator last, T init, BinaryOp op) {
    for(; first != last ; ++first) {
        init = op(init, *first);
    }
    return init;
}


// accumulate
// 1: 计算相邻元素的差值，结果保存在result起始的区间上，result[0] = elem[0], result[n] = elem[n] - elem[n-1]
// 2: 自定义二元函数，op(elem[n], elem[n-1])
template <class InputIterator, class OutputIterator>
OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator result) {
    if(first == last) return result;
    *result = *first;   // 第一个元素
    auto value = *first;        // 上一个元素
    while(++first != last) {
        auto tmp = *first;
        *++result = tmp - value;
        value = tmp;
    }
    return ++result;
}

template <class InputIterator, class OutputIterator, class BinaryOp>
OutputIterator adjacent_difference(InputIterator first, InputIterator last, OutputIterator result, BinaryOp op) {
    if(first == last) return result;
    *result = *first;   // 第一个元素
    auto value = *first;        // 上一个元素
    while(++first != last) {
        auto tmp = *first;
        *++result = op(tmp, value);
        value = tmp;
    }
    return ++result;
}


// inner_product
// 1: 以init为初值，计算两个区间的内积
// 2: 自定义operator+ 和 operator*
template <class InputIterator1, class InputIterator2, class T>
T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init) {
    for(; first1 != last1 ; ++first1, ++first2) {
        init = init + (*first1 * *first2);
    }
    return init;
}

// op1 == +   op1 == *
template <class InputIterator1, class InputIterator2, class T, class BinaryOp1, class BinaryOp2>
T inner_product(InputIterator1 first1, InputIterator1 last1, InputIterator2 first2, T init, BinaryOp1 op1, BinaryOp2 op2) {
    for(; first1 != last1 ; ++first1, ++first2) {
        init = op1(init, op2(*first1, *first2));
    }
    return init;
}


// iota
// 填充[first, last)，以value为初值开始递增
template <class ForwardIterator, class T>
void iota(ForwardIterator first, ForwardIterator last, T value) {
    while(first != last) {
        *first = value++;
        ++first;
    }
}


// partial_sum
// 1: 计算[first, last)区间的前缀和(包括当前位置)
// 2: 自定义二元操作
template <class InputIterator, class OutputIterator>
OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result) {
    if(first == last) return result;
    auto value = *first;
    *result = *first;
    while(++first != last) {
        value = value + *first;
        *++result = value;
    }
    return ++result;
}

template <class InputIterator, class OutputIterator, class BinaryOp>
OutputIterator partial_sum(InputIterator first, InputIterator last, OutputIterator result, BinaryOp op) {
    if(first == last) return result;
    auto value = *first;
    *result = *first;
    while(++first != last) {
        value = op(value, *first);
        *++result = value;
    }
    return ++result;
}

}

#endif