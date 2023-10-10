#ifndef __ALGO_H__
#define __ALGO_H__

#include <cstddef>

#include "algobase.h"
#include "heap_algo.h"
#include "functional.h"
#include "iterator.h"

// 由于算法太多了，这里有所选择的实现了一部分
// count    count_if    find    find_if     for_each    reverse     rotate  partition   max_element     min_element
// unique   lower_bound upper_boud  next_permutation    prev_permutation    merge       sort*

namespace mystl {

/*****************************************************************************************/
// count
// 在[first, last)中查找value的个数，operator==
/*****************************************************************************************/
template <class InputIterator, class T>
size_t count(InputIterator first, InputIterator last, const T& value) {
    size_t n = 0;
    for(; first != last ; ++first) {
        if(*first == value) ++n;
    }
    return n;
}



/*****************************************************************************************/
// count_if
// 在[first, last)中查找满足 unary_pred的，返回true的个数
/*****************************************************************************************/
template <class InputIterator, class UnaryPredicate>
size_t count_if(InputIterator first, InputIterator last, UnaryPredicate unary_pred) {
    size_t n = 0;
    for(; first != last ; ++first) {
        if(unary_pred(*first))  ++n;
    }
    return n;
}


/*****************************************************************************************/
// find
// 在[first, last)中找到第一个等于value的元素，返回其迭代器
/*****************************************************************************************/
template <class InputIterator, class T>
InputIterator find(InputIterator first, InputIterator last, const T& value) {
    while(first != last && *first != value) {
        ++first;
    }
    return first;   // 如果没找到，那就是返回last
}


/*****************************************************************************************/
// find_if
// 在[first, last)中找到第一个满足 unary_pred 为true的元素，返回迭代器
/*****************************************************************************************/
template <class InputIterator, class UnaryPredicate>
InputIterator find_if(InputIterator first, InputIterator last, UnaryPredicate unary_pred) {
    while(first != last && unary_pred(*first)) {
        ++first;
    }
    return first;
}


/*****************************************************************************************/
// for_each
// 在[first, last)中，对每一个元素执行operator()，但不改变其元素内容，可以返回值，但是会被忽略
/*****************************************************************************************/
template <class InputIterator, class Function>
Function for_each(InputIterator first, InputIterator last, Function f) {
    while(first != last) {
        f(*first++);
    }
    return f;
}



/*****************************************************************************************/
// reverse
// 将[first, last)翻转
/*****************************************************************************************/
template <class BidirectionIterator>
void reverse_dispatch(BidirectionIterator first, BidirectionIterator last, bidirectional_iterator_tag) {
    while(true) {
        if(first == last || first == --last)    return; // 在这里提前--
        mystl::swap(*first++, *last);
    }
}

template <class RandomIterator>
void reverse_dispatch(RandomIterator first, RandomIterator last, random_access_iterator_tag) {
    while(first < last) {
        mystl::swap(*first++, *--last);
    }
}


template <class BidirectionIterator>
void reverse(BidirectionIterator first, BidirectionIterator last) {
    reverse_dispatch(first, last, iterator_category(first));
}


/*****************************************************************************************/
// rotate
// 将[first, middle)内的元素，和[middle, last) 元素互换，返回交换后的middle的位置，不是指原有的middle
// 而是指中间的分割点
/*****************************************************************************************/
template <class ForwardIterator>
ForwardIterator 
rotate_dispatch(ForwardIterator first, ForwardIterator middle, ForwardIterator last, forward_iterator_tag) {
    // 这个算法有点像分割子问题一样，新的middle则是新的需要旋转的中间点，根据两边的长短，来调整middle
    auto first2 = middle;
    do {
        mystl::swap(*first++, *first2++);
        if(first == middle) {
            middle = first2;    // 这种情况只出现于，前段比后端短的情况
        }
    }while(first2 != last);     // 把后半段移动到前面半段，于是前面半段的次序就乱了

    auto new_middle = first;    // 这里肯定是新的分割点
    first2 = middle;            // 继续调整新的rotate
    // 子问题：[first, middle) [middle, last) 中间可能会调整middle的位置，如果前段短，则要调整middle
    while(first2 != last) {
        mystl::swap(*first++, *first2++);
        if(first == middle) {
            middle = first2;    // 新的middle
        }else if(first2 == last){
            // 后短比较短，然后从middle处，继续变成新的[first, middle) [middle, last)
            first2 = middle;
        }
    }
    return new_middle;
} 

// 采用reverse的方法
template <class BidirectionIterator>
BidirectionIterator 
rotate_dispatch(BidirectionIterator first, BidirectionIterator middle, BidirectionIterator last, bidirectional_iterator_tag) {
    mystl::reverse_dispatch(first, middle, bidirectional_iterator_tag());
    mystl::reverse_dispatch(middle, last, bidirectional_iterator_tag());
    while(first != middle && middle != last) {
        mystl::swap(*first++, *--last);
    }
    if(first == middle) {
        // 后半部分长
        mystl::reverse_dispatch(middle, last, bidirectional_iterator_tag());
        return last;    // 此时last是新的分割点
    }else {
        mystl::reverse_dispatch(first, middle, bidirectional_iterator_tag());
        return first;
    }
}

// gcd(m, n) == gcd(n, m % n) m > n . 当其中一个为0时，则就是答案 
template <class Number>
Number rgcd(Number m, Number n) {
    while (n != 0) {
        Number t = m % n;
        // 下一轮迭代
        m = n;
        n = t;
    }
    return m;
}

// 直接用bidirection的
/* 
template <class RandomIterator>
RandomIterator
rotate_dispatch(RandomIterator first, RandomIterator middle, RandomIterator last, random_access_iterator_tag) {} 
*/

template <class ForwardIterator>
ForwardIterator rotate(ForwardIterator first, ForwardIterator middle, ForwardIterator last) {
    if(first == middle) 
        return last;
    if(middle == last)
        return first;
    return rotate_dispatch(first, middle, last, iterator_category(first));
}


/**********************************************************************************************/
// partition
// 对[first, last)区间重排，对于unary_predicate满足为true的元素会放到区间前段，然后返回分割点，不稳定
/*********************************************************************************************/
template <class BidirectionIterator, class UnaryPredicate>
BidirectionIterator
partition(BidirectionIterator first, BidirectionIterator last, UnaryPredicate unary_pred) {
    while(true) {
        // 移动first指针，找到第一个不满足unary_pred的
        while(true) {
            if(first == last) return first; // 区间找完了
            else if(unary_pred(*first)) ++first;
            else break;     // 找到了
        }

        --last;     // 回溯一下
        while(true) {
            if(first == last) return first;
            else if(!unary_pred(*last)) --last;
            else break;     // 找到了满足的
        }
        
        // 执行交换，指针移动
        mystl::swap(*first, *last);
        ++first;
    }
    // 在循环内部return了
}


/**********************************************************************************************/
// max_element
// 寻找[first, last)中的最大元素，并返回其迭代器，如果有多个最大的，返回第一个
/*********************************************************************************************/
template <class ForwardIterator>
ForwardIterator max_element(ForwardIterator first, ForwardIterator last) {
    if(first == last) return first;
    ForwardIterator result = first;
    while(first != last) {
        if(*result < *first) {
            result = first;
        }
    }
    return result;
}

// 自定义最大的比较规则
template <class ForwardIterator, class Compare>
ForwardIterator max_element(ForwardIterator first, ForwardIterator last, Compare comp) {
    if(first == last) return first;
    ForwardIterator result = first;
    while(first != last) {
        if(comp(*result, *first)) {
            result = first;
        }
    }
    return result;
}


/**********************************************************************************************/
// min_element
// 寻找[first, last)中的最小元素，并返回其迭代器，如果有多个最小的，返回第一个
/*********************************************************************************************/
template <class ForwardIterator>
ForwardIterator min_element(ForwardIterator first, ForwardIterator last) {
    if(first == last) return first;
    ForwardIterator result = first;
    while(first != last) {
        if(*first < *result) {
            result = first;
        }
    }
    return result;
}

template <class ForwardIterator, class Compare>
ForwardIterator min_element(ForwardIterator first, ForwardIterator last, Compare comp) {
    if(first == last) return first;
    ForwardIterator result = first;
    while(first != last) {
        if(comp(*first, *result)) {
            result = first;
        }
    }
    return result;
}


/**********************************************************************************************/
// uniuqe
// 移除[first, last)区间内的相邻重复元素，注意是相邻，并不能真正意义上的删除，返回新的尾端，可以配合erase使用
/*********************************************************************************************/
template <class ForwardIterator>
ForwardIterator unique(ForwardIterator first, ForwardIterator last) {
    if(first == last)   return first;
    ForwardIterator result = first;
    // result指向已经去除相邻元素的最后一个
    while(++first != last) {
        if(*result != *first) {
            *++result = *first;     // 不同则记录
        }
        // 如果相同。只有first移动
    }
    return ++result;
}


/*****************************************************************************************/
// lower_bound
// 在[first, last)中查找第一个不小于 value 的元素，并返回指向它的迭代器，若没有则返回 last
/*****************************************************************************************/
template <class InputIterator, class T>
InputIterator lower_bound_dispatch(InputIterator first, InputIterator last, const T& value, input_iterator_tag) {
    auto len = last - first;
    InputIterator mid;
    while(len > 0) {
        auto half = len >> 1;
        mid = first;
        mystl::advance(mid, half);  // mid = first + half;
        if(*mid < value) {
            // 下界收缩
            first = mid;
            ++first;
            len -= half + 1;
        }else {
            len = half;
        }
    }
    return first;
}

template <class RandomIterator, class T>
RandomIterator lower_bound_dispatch(RandomIterator first, RandomIterator last, const T& value, random_access_iterator_tag) {
    auto len = last - first;
    RandomIterator mid;
    while(len > 0) {
        auto half = len >> 1;
        mid = first + half;
        if(*mid < value) {
            // 下界收缩
            first = mid;
            ++first;
            len -= half + 1;
        }else {
            // == 的情况上界收缩
            len = half;
        }
    }
    return first;
}


template <class Iterator, class T>
Iterator lower_bound(Iterator first, Iterator last, const T& value) {
    return lower_bound_dispatch(first, last, value, iterator_category(first));
}   


template <class InputIterator, class T, class Compare>
InputIterator lower_bound_dispatch(InputIterator first, InputIterator last, const T& value, input_iterator_tag, Compare comp) {
    auto len = last - first;
    InputIterator mid;
    while(len > 0) {
        auto half = len >> 1;
        mid = first;
        mystl::advance(mid, half);  // mid = first + half;
        if(comp(*mid, value)) {
            // 下界收缩
            first = mid;
            ++first;
            len -= half + 1;
        }else {
            len = half;
        }
    }
    return first;
}

template <class RandomIterator, class T, class Compare>
RandomIterator lower_bound_dispatch(RandomIterator first, RandomIterator last, const T& value, random_access_iterator_tag, Compare comp) {
    auto len = last - first;
    RandomIterator mid;
    while(len > 0) {
        auto half = len >> 1;
        mid = first + half;
        if(comp(*mid, value)) {
            // 下界收缩
            first = mid;
            ++first;
            len -= half + 1;
        }else {
            // == 的情况上界收缩
            len = half;
        }
    }
    return first;
}

template <class Iterator, class T, class Compare>
Iterator lower_bound(Iterator first, Iterator last, const T& value, Compare comp) {
    return lower_bound_dispatch(first, last, value, iterator_category(first), comp);
}  


/*****************************************************************************************/
// upper_bound
// 在[first, last)中查找第一个大于 value 的元素，并返回指向它的迭代器，若没有则返回 last
/*****************************************************************************************/
template <class InputIterator, class T>
InputIterator uupper_bound_dispatch(InputIterator first, InputIterator last, const T& value, input_iterator_tag) {
    auto len = last - first;
    InputIterator mid;
    while(len > 0) {
        auto half = len >> 1;
        mid = first;
        mystl::advance(mid, half);      // first n
        if(!(value < *mid)) {
            first = mid;
            ++first;
            len -= half + 1;
        }else {
            len = half;
        }
    }
    return first;
}

template <class RandomIterator, class T>
RandomIterator upper_bound_dispatch(RandomIterator first, RandomIterator last, const T& value, random_access_iterator_tag) {
    auto len = last - first;
    RandomIterator mid;
    while(len > 0) {
        auto half = len >> 1;
        mid = first + half;
        if(!(value < *mid)) {
            // *mid <= value   !(value < *mid)
            first = mid;
            ++first;
            len -= half + 1;
        }else {
            half = len;
        }
    }
    return first;
}

template <class Iterator, class T>
Iterator upper_bound(Iterator first, Iterator last, const T& value) {
    return upper_bound_dispatch(first, last, value, iterator_category(first));
}


template <class InputIterator, class T, class Compare>
InputIterator uupper_bound_dispatch(InputIterator first, InputIterator last, const T& value, input_iterator_tag, Compare comp) {
    auto len = last - first;
    InputIterator mid;
    while(len > 0) {
        auto half = len >> 1;
        mid = first;
        mystl::advance(mid, half);      // first n
        if(!comp(value, *mid)) {
            first = mid;
            ++first;
            len -= half + 1;
        }else {
            len = half;
        }
    }
    return first;
}

template <class RandomIterator, class T, class Compare>
RandomIterator upper_bound_dispatch(RandomIterator first, RandomIterator last, const T& value, random_access_iterator_tag, Compare comp) {
    auto len = last - first;
    RandomIterator mid;
    while(len > 0) {
        auto half = len >> 1;
        mid = first + half;
        if(!comp(value, *mid)) {
            // *mid <= value   !(value < *mid)
            first = mid;
            ++first;
            len -= half + 1;
        }else {
            half = len;
        }
    }
    return first;
}

template <class Iterator, class T, class Compare>
Iterator upper_bound(Iterator first, Iterator last, const T& value, Compare comp) {
    return upper_bound_dispatch(first, last, value, iterator_category(first), comp);
}


/*****************************************************************************************/
// next_permutation
// 在[first, last)所呈现的序列的下一个排列组合，如果没有下一排列组合，返回false，否则返回false，更改了元素
// 如 12345 --> 12354(true)    23541 --> 24135(true)    54321 --> 12345(false)   
/*****************************************************************************************/
template <class BidirectionIterator>
bool next_permutation(BidirectionIterator first, BidirectionIterator last) {
    if(first == last) return false;     // 长度为0
    BidirectionIterator i = last;
    if(first == --i) return false;      // 长度为1

    // 找到一个顺序对[i, ii]，然后从[ii, last)倒序找到第一个大于i的，交换，然后把[ii, last)给reverse
    for(;;) {
        BidirectionIterator ii = i;
        if(*--i < *ii) {    // i在这里移动
            BidirectionIterator j = last;
            while(!(*i < *--j)) {}
            // j指向倒序第一个比i大的
            mystl::swap(*i, *j);
            mystl::reverse(ii, last);
            return true;
        }

        // 没有找到顺序对，说明是最后一种排列组合
        if(i == first) {
            mystl::reverse(first, last);
            return false;
        }
    }
}

template <class BidirectionIterator, class Compare>
bool next_permutation(BidirectionIterator first, BidirectionIterator last, Compare comp) {
    if(first == last) return false;     // 长度为0
    BidirectionIterator i = last;
    if(first == --i) return false;      // 长度为1

    // 找到一个顺序对[i, ii]，然后从[ii, last)倒序找到第一个大于i的，交换，然后把[ii, last)给reverse
    for(;;) {
        BidirectionIterator ii = i;
        if(comp(*--i, *ii)) {    // i在这里移动
            BidirectionIterator j = last;
            while(!comp(*i, *--j)) {}
            // j指向倒序第一个比i大的
            mystl::swap(*i, *j);
            mystl::reverse(ii, last);
            return true;
        }

        // 没有找到顺序对，说明是最后一种排列组合
        if(i == first) {
            mystl::reverse(first, last);
            return false;
        }
    }
}


/*****************************************************************************************/
// prev_permutation
// 在[first, last)所呈现的序列的上一个排列组合，如果没有上一排列组合，返回false，否则返回false，更改了元素 
/*****************************************************************************************/
template <class BidirectionIterator>
bool prev_permutation(BidirectionIterator first, BidirectionIterator last) {
    if(first == last) return false;     // 长度为0
    BidirectionIterator i = last;
    if(first == --i) return false;      // 长度为1

    // 找到一个逆序对[i, ii]，然后从[ii, last)倒序找到第一个小于i的，交换，然后把[ii, last)给reverse
    for(;;) {
        BidirectionIterator ii = i;
        if(*ii < *--i) {    // i在这里移动
            BidirectionIterator j = last;
            while(!(*--j < *i)) {}
            // j指向倒序第一个比i小的
            mystl::swap(*i, *j);
            mystl::reverse(ii, last);
            return true;
        }

        if(i == first) {
            mystl::reverse(first, last);
            return false;
        }
    }
}

template <class BidirectionIterator, class Compare>
bool prev_permutation(BidirectionIterator first, BidirectionIterator last, Compare comp) {
    if(first == last) return false;     // 长度为0
    BidirectionIterator i = last;
    if(first == --i) return false;      // 长度为1

    // 找到一个逆序对[i, ii]，然后从[ii, last)倒序找到第一个小于i的，交换，然后把[ii, last)给reverse
    for(;;) {
        BidirectionIterator ii = i;
        if(comp(*ii, *--i)) {    // i在这里移动
            BidirectionIterator j = last;
            while(!comp(*--j, *i)) {}
            mystl::swap(*i, *j);
            mystl::reverse(ii, last);
            return true;
        }

        if(i == first) {
            mystl::reverse(first, last);
            return false;
        }
    }
}

/*****************************************************************************************/
// median
// 找出三个值的中间值
// 思路按照 left < mid          mid <= left 分支来进行      通过right在其中的位置来判断
/*****************************************************************************************/
template <class T>
const T& median(const T& left, const T& mid, const T& right) {
    if(left < mid) {
        if(mid < right)
            return mid;     // left < mid < right
        else if(left < right) 
            return right;   // left < right <= mid
        else 
            return left;    // right <= left < mid
    }else if (left < right)
        return left;        // mid <= left < right
    else if(mid < right)
        return right;       // mid < right <= left 
    else
        return mid;         // right <= mid <= left
}

template <class T, class Compare>
const T& median(const T& left, const T& mid, const T& right, Compare comp) {
    if(comp(left, mid)) {
        if(comp(mid, right))
            return mid;     // left < mid < right
        else if(comp(left, right)) 
            return right;   // left < right <= mid
        else 
            return left;    // right <= left < mid
    }else if (comp(left, right))
        return left;        // mid <= left < right
    else if(comp(mid, right))
        return right;       // mid < right <= left 
    else
        return mid;         // right <= mid <= left
}


/*****************************************************************************************/
// merge
// 将两个经过排序(有序)的集合 S1 和 S2 合并起来置于另一段空间，返回一个迭代器指向最后一个元素的下一位置
/*****************************************************************************************/
template <class InputIterator1, class InputIterator2, class OutputIterator>
OutputIterator
merge(InputIterator1 first1, InputIterator1 last1,
      InputIterator2 first2, InputIterator2 last2,
      OutputIterator result) {
    while(first1 != last1 && first2 != last2) {
        if(*first1 < *first2) {
            *result = *first1;
            ++first1;
        }else {
            // ==随便
            *result = *first2;
            ++first2;
        }
        ++result;
    }
    return mystl::copy(first1, last1, mystl::copy(first2, last2, result));
}

template <class InputIterator1, class InputIterator2, class OutputIterator, class Compare>
OutputIterator
merge(InputIterator1 first1, InputIterator1 last1,
      InputIterator2 first2, InputIterator2 last2,
      OutputIterator result, Compare        comp) {
    while(first1 != last1 && first2 != last2) {
        if(comp(*first1, *first2)) {
            *result = *first1;
            ++first1;
        }else {
            // ==随便
            *result = *first2;
            ++first2;
        }
        ++result;
    }
    return mystl::copy(first1, last1, mystl::copy(first2, last2, result));
}

/*****************************************************************************************/
// partial_sort
// 对整个序列做部分排序，保证较小的 N 个元素以递增顺序置于[first, first + N)中
/*****************************************************************************************/
template <class RandomIterator>
void partial_sort(RandomIterator first, RandomIterator middle, RandomIterator last) {
    make_heap(first, middle);
    for(auto i = middle ; i < last ; ++i) {
        if(*i < *first) {
            // 如果说*i比最大值小，则交换两者位置，再重新在[first, middle)形成堆。循环结束后，[first, middle)就是最小的区间了
            pop_heap_aux(first, middle, i, *i, difference_type(first));
        }
    }
    // 再把前n小的元素给排序
    sort_heap(first, middle);
}

template <class RandomIterator, class Compare>
void partial_sort(RandomIterator first, RandomIterator middle, RandomIterator last, Compare comp) {
    make_heap(first, middle);
    for(auto i = middle ; i < last ; ++i) {
        if(comp(*i, *first)) {
            // 如果说*i比最大值小，则交换两者位置，再重新在[first, middle)形成堆。循环结束后，[first, middle)就是最小的区间了
            pop_heap_aux(first, middle, i, *i, difference_type(first), comp);
        }
    }
    // 再把前n小的元素给排序
    sort_heap(first, middle, comp);
}


/*****************************************************************************************/
// sort
// 将[first, last)区间进行排序，要求传入的迭代器为random_tag
// 在STL中的sort算法，并不是完全的QuickSort。其思想主要是，在数据量大的时候采用QuickSort，分段递归排序。
// 一旦分段后的数据量小于某个门槛(stl_threshold)，为了防止小数据量造成的递归代价，改用InsertSort。
// 如果递归过于深，则改用HeapSort。
/*****************************************************************************************/
const static size_t threshold = 16;        // 小型区间的大小，一旦区间小于这个值，则改变使用插入排序

// 该函数用于控制分割递归恶化的情况，为递归的深度。2^k <= n，返回k。
// 如果一个n == 40，那么k == 5，最多允许分割5 * 2层
template <class Size>
Size lg(Size n) {
    Size k = 0;
    // 16 8 4 2 各记一次
    for(; n > 1 ; n >>= 1) {
        ++k;
    }
    return k;
}

// [first, cur) <= pivot        [cur, last) >= pivot 
template <class RandomIterator, class T>
RandomIterator
unchecked_partition(RandomIterator first, RandomIterator last, const T& pivot) {
    while(true) {
        while(*first < pivot)
            ++first;
        --last;
        while(pivot < *last)
            --last;
        if(!(first < last)) 
            return first;       // first == last + 1
        mystl::swap(*first, *last);
        ++first;
    }
}

template <class RandomIterator, class Size>
void intro_sort(RandomIterator first, RandomIterator last, Size depth_limit) {
    while(last - first > threshold) {
        if(depth_limit == 0) {
            // 分支恶化，采用堆排序
            partial_sort(first, last, last);
            return;
        }
        --depth_limit;
        // mid_of_three 找首 中 尾三个值的中间值，防止分割区间退化
        auto mid = median(*(first), *(first + (last - first) / 2), *(last - 1));
        auto cut = unchecked_partition(first, last, mid);     // 将[first, last)分割，左半部分 <= pivot，右半部分 > pivot，返回分割区间
        intro_sort(cut, last, depth_limit);     // 递归分割右半部分
        last = cut; // 循环处理左半部分
    }
    // 如果区间长度<= thresh_old 直接返回了，什么也不干
}

// 插入排序辅助函数 unchecked_linear_insert, 将value插入到 [first, last) 之前的区间，没有边界检查
template <class RandomIterator, class T>
void unchecked_linear_insert(RandomIterator last, const T& value) {
    RandomIterator next = last;
    --next;
    while(value < *next) {
        // 往后移动
        *last = *next;
        last = next;
        --next;
    }
    // 最后value该在最终的位置last，因为*next <= value了
    *last = value;
}


// 插入排序函数 insertion_sort
template <class RandomIterator>
void insertion_sort(RandomIterator first, RandomIterator last) {
    if(first == last) return;

    for(auto i = first + 1 ; i != last ; ++i) {
        auto value = *i;
        if(value < *first) {
            // 插在头部
            mystl::copy_backward(first, i, i + 1);  // 总体向后移动
            *first = value;
        }else {
            unchecked_linear_insert(i, value);      // 将value插入到[start, i) 区间中
        }
    }
}

template <class RandomIterator>
void unchecked_insertion_sort(RandomIterator first, RandomIterator last) {
    for(auto i = first ; i != last ; ++i) {
        unchecked_linear_insert(i, *i);
    }
}


// 最终插入排序函数 final_insertion_sort
template <class RandomIterator>
void final_insertion_sort(RandomIterator first, RandomIterator last) {
    if(last - first > threshold) {
        // 分成两端排序，先保证第一段局部有序，然后后面每一个元素，都插入
        insertion_sort(first, first + threshold);
        unchecked_insertion_sort(first + threshold, last);
    }else {
        // 小于阈值，直接插排了
        insertion_sort(first, last);
    }
}

template <class RandomIterator>
void sort(RandomIterator first, RandomIterator last) {
    if(first != last) {
        // 内省式排序，将区间分为一个个的小区间，然后对整体进行插入排序
        intro_sort(first, last, lg(last - first) * 2);
        final_insertion_sort(first, last);
    }
}


// ------------Compare版本-------------------
template <class RandomIterator, class T, class Compare>
RandomIterator
unchecked_partition(RandomIterator first, RandomIterator last, const T& pivot, Compare comp) {
    while(true) {
        while(comp(*first, pivot))
            ++first;
        --last;
        while(comp(pivot, *last))
            --last;
        if(!(first < last)) 
            return first;       // first == last + 1
        mystl::swap(*first, *last);
        ++first;
    }
}

template <class RandomIterator, class Size, class Compare>
void intro_sort(RandomIterator first, RandomIterator last, Size depth_limit, Compare comp) {
    while(last - first > threshold) {
        if(depth_limit == 0) {
            // 分支恶化，采用堆排序
            partial_sort(first, last, last, comp);
            return;
        }
        --depth_limit;
        // mid_of_three 找首 中 尾三个值的中间值，防止分割区间退化
        auto mid = median(*(first), *(first + (last - first) / 2), *(last - 1)/* , comp */);
        auto cut = unchecked_partition(first, last, mid, comp);     // 将[first, last)分割，左半部分 <= pivot，右半部分 > pivot，返回分割区间
        intro_sort(cut, last, depth_limit, comp);     // 递归分割右半部分
        last = cut; // 循环处理左半部分
    }
    // 如果区间长度<= thresh_old 直接返回了，什么也不干
}

// 插入排序辅助函数 unchecked_linear_insert, 将value插入到 [first, last) 之前的区间，没有边界检查
template <class RandomIterator, class T, class Compare>
void unchecked_linear_insert(RandomIterator last, const T& value, Compare comp) {
    RandomIterator next = last;
    --next;
    while(comp(value, *next)) {
        // 往后移动
        *last = *next;
        last = next;
        --next;
    }
    // 最后value该在最终的位置last，因为*next <= value了
    *last = value;
}


// 插入排序函数 insertion_sort
template <class RandomIterator, class Compare>
void insertion_sort(RandomIterator first, RandomIterator last, Compare comp) {
    if(first == last) return;

    for(auto i = first + 1 ; i != last ; ++i) {
        auto value = *i;
        if(comp(value, *first)) {
            // 插在头部
            mystl::copy_backward(first, i, i + 1);  // 总体向后移动
            *first = value;
        }else {
            unchecked_linear_insert(i, value, comp);      // 将value插入到[start, i) 区间中
        }
    }
}

template <class RandomIterator, class Compare>
void unchecked_insertion_sort(RandomIterator first, RandomIterator last, Compare comp) {
    for(auto i = first ; i != last ; ++i) {
        unchecked_linear_insert(i, *i, comp);
    }
}


// 最终插入排序函数 final_insertion_sort
template <class RandomIterator, class Compare>
void final_insertion_sort(RandomIterator first, RandomIterator last, Compare comp) {
    if(last - first > threshold) {
        // 分成两端排序，先保证第一段局部有序，然后后面每一个元素，都插入
        insertion_sort(first, first + threshold, comp);
        unchecked_insertion_sort(first + threshold, last, comp);
    }else {
        // 小于阈值，直接插排了
        insertion_sort(first, last, comp);
    }
}

template <class RandomIterator, class Compare>
void sort(RandomIterator first, RandomIterator last, Compare comp) {
    if(first != last) {
        // 内省式排序，将区间分为一个个的小区间，然后对整体进行插入排序
        intro_sort(first, last, lg(last - first) * 2, comp);
        final_insertion_sort(first, last, comp);
    }
}


} // end of namespace mystl

#endif
