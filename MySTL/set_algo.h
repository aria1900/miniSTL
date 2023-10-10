#ifndef __SET_ALGO_H__
#define __SET_ALGO_H__

#include "algobase.h"
#include "iterator.h"

// 本文件实现了有关set集合的算法，并集、交集、差集和对称差集，要求两个区间有序

namespace mystl {

// set_union 并集，计算 S1∪S2 的结果并保存到 result 中，返回一个迭代器指向输出结果的尾部
// 函数说明：1,3,5,7,9,11   1,1,2,3,5,8,13      并集后: 1,1,3,5,7,8,9,11,13  
// 相同元素，取个数多的
template <class InputIterator1, class InputIterator2, class OuputIterator>
OuputIterator set_union(InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2,
                        OuputIterator result) {
    while(first1 != last1 && first2 != last2) {
        if(*first1 < *first2) {
            *result = *first1;
            ++first1;
        }else if(*first2 < *first1) {
            *result = *first2;
            ++first2;
        }else {
            // == 都移动迭代器
            *result = *first1;
            ++first1;
            ++first2;
        }

        ++result;
    }
    return mystl::copy(first1, last1, mystl::copy(first2, last2, result));
}

template <class InputIterator1, class InputIterator2, class OuputIterator, class Compare>
OuputIterator set_union(InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2,
                        OuputIterator result, Compare comp) {
    while(first1 != last1 && first2 != last2) {
        if(comp(*first1, *first2)) {
            *result = *first1;
            ++first1;
        }else if(comp(*first2 < *first1)) {
            *result = *first2;
            ++first2;
        }else {
            // == 都移动迭代器
            *result = *first1;
            ++first1;
            ++first2;
        }

        ++result;
    }
    // 将剩余元素拷贝到result上
    return mystl::copy(first1, last1, mystl::copy(first2, last2, result));
}


// set_intersection 交集，计算 S1 ∩ S2 的结果并保存到 result 中，返回一个迭代器指向输出结果的尾部
// 函数说明：1,3,5,7,9,11   1,1,2,3,5,8,13      交集后: 1,3,5  
// 相同元素，取个数少的
template <class InputIterator1, class InputIterator2, class OuputIterator>
OuputIterator set_intersection(InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2,
                        OuputIterator result) {
    while(first1 != last1 && first2 != last2) {
        if(*first1 < *first2) {
            ++first1;
        }else if(*first2 < *first1) {
            ++first2;
        }else {
            // == 只有相等才赋值
            *result = *first1;
            ++result;
            ++first1;
            ++first2;
        }
    }
    // 结束后如果有一方还有剩余，也不需要再拷贝了，因为注定不是交集
    return result;
}

template <class InputIterator1, class InputIterator2, class OuputIterator, class Compare>
OuputIterator set_intersection(InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2,
                        OuputIterator result, Compare comp) {
    while(first1 != last1 && first2 != last2) {
        if(comp(*first1, *first2)) {
            ++first1;
        }else if(comp(*first2, *first1)) {
            ++first2;
        }else {
            // == 只有相等才赋值
            *result = *first1;
            ++result;
            ++first1;
            ++first2;
        }
    }
    // 结束后如果有一方还有剩余，也不需要再拷贝了，因为注定不是交集
    return result;
}


// set_difference 差集，计算 S1 - S2 的结果并保存到 result 中，返回一个迭代器指向输出结果的尾部
// 函数说明：1,3,5,7,9,11   1,1,2,3,5,8,13      差集后: 7,9,11  
// 出现与S1，但没有在S2的
// 如果相同，那么肯定是S1_value > S2_value, 取差值
template <class InputIterator1, class InputIterator2, class OuputIterator>
OuputIterator set_difference(InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2,
                        OuputIterator result) {
    while(first1 != last1 && first2 != last2) {
        if(*first1 < *first2) {
            *result = *first1;
            ++first1;
            ++result;
        }else if(*first2 < *first1) {
            // 移动指针
            ++first2;
        }else {
            // 相等都移动
            ++first1;
            ++first2;
        }
    }
    // 如果S1有剩余，S2没有剩余，说明S1剩余的全是S2没有的
    // 如果S2有剩余，S1没有剩余，说明S2剩余的全是S1没有的，不是答案
    return mystl::copy(first1, last1, result);
}

template <class InputIterator1, class InputIterator2, class OuputIterator, class Compare>
OuputIterator set_difference(InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2,
                        OuputIterator result, Compare comp) {
    while(first1 != last1 && first2 != last2) {
        if(comp(*first1, *first2)) {
            *result = *first1;
            ++first1;
            ++result;
        }else if(comp(*first2, *first1)) {
            // 移动指针
            ++first2;
        }else {
            // 相等都移动
            ++first1;
            ++first2;
        }
    }
    // 如果S1有剩余，S2没有剩余，说明S1剩余的全是S2没有的
    // 如果S2有剩余，S1没有剩余，说明S2剩余的全是S1没有的，不是答案
    return mystl::copy(first1, last1, result);
}


// set_symmetric_difference 对称差集，计算 (S1-S2)∪(S2-S1) 的结果并保存到 result 中，返回一个迭代器指向输出结果的尾部
// 函数说明：1,3,5,7,9,11   1,1,2,3,5,8,13      差集后: 1,2,7,8,9,11,13 
// 出现在S1，但没有在S2的 和 出现在S2，但没有在S1的
// 如果元素相同，max - min，来自max的集合的相同元素
template <class InputIterator1, class InputIterator2, class OuputIterator>
OuputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2,
                        OuputIterator result) {
    while(first1 != last1 && first2 != last2) {
        if(*first1 < *first2) {
            *result = *first1;
            ++result;
            ++first1;
        }else if(*first2 < *first1) {
            *result = *first2;
            ++result;
            ++first2;
        }else {
            // == 则跳过
            ++first1;
            ++first2;
        }
    }
    // 把剩余元素拷贝到result即可，因为剩余的一定是另一个元素没有的
    return mystl::copy(first1, last1, mystl::copy(first2, last2, result));
}

template <class InputIterator1, class InputIterator2, class OuputIterator, class Compare>
OuputIterator set_symmetric_difference(InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2,
                        OuputIterator result, Compare comp) {
    while(first1 != last1 && first2 != last2) {
        if(comp(*first1, *first2)) {
            *result = *first1;
            ++result;
            ++first1;
        }else if(comp(*first2, *first1)) {
            *result = *first2;
            ++result;
            ++first2;
        }else {
            // == 则跳过
            ++first1;
            ++first2;
        }
    }
    // 把剩余元素拷贝到result即可，因为剩余的一定是另一个元素没有的
    return mystl::copy(first1, last1, mystl::copy(first2, last2, result));
} 

}
#endif