#ifndef __HEAP_ALGO_H__
#define __HEAP_ALGO_H__

#include "iterator.h"

// 头文件包括heap的四个算法 : push_heap 、pop_heap 、make_heap 、 sort_heap

namespace mystl {
/*
*   说明一下push_heap函数，接收一个[first, last)，将last - 1的元素上溯，每次与parent比较
*   如果parent小，则把parent拉下来，最后知道parent不小于holeIndex，则该位置就是push的元素该待的位置
*/


// ************************************************************************************
// push_heap接收[first, last)区间，表示heap的首尾，并且新元素已经添加到容器尾部，进行上溯操作
// ************************************************************************************

// holeIndex表示插入元素的位置
template <class RandomIterator, class Distance, class T>
void push_heap_aux(RandomIterator first, Distance holeIndex, Distance topIndex, T value) {
    Distance parent = (holeIndex - 1) / 2;
    while(holeIndex > topIndex && *(first + parent) < value) {
        // 大根堆，只要parent小，就把parent拉下来
        *(first + holeIndex) = *(first + parent);
        holeIndex = parent;
        parent = (holeIndex - 1) / 2;
    }
    *(first + holeIndex) = value;  // holeIndex == 0 || *(first + parent) >= value, 也就是holeIndex到达指定位置
}

// 该函数获取Distance类型
template <class RandomIterator, class Distance>
void push_heap_d(RandomIterator first, RandomIterator last, Distance *) {
    push_heap_aux(first, static_cast<Distance>(last - first - 1), static_cast<Distance>(0), *(last - 1));
}

template <class RandomIterator>
void push_heap(RandomIterator first, RandomIterator last) {
    push_heap_d(first, last, difference_type(first));
}


// compare版本

template <class RandomIterator, class Distance, class T, class Compare>
void push_heap_aux(RandomIterator first, Distance holeIndex, Distance topIndex, T value, Compare comp) {
    Distance parent = (holeIndex - 1) / 2;
    while(holeIndex > topIndex && comp(*(first + parent), value) ) {
        // 大根堆，只要parent小，就把parent拉下来
        *(first + holeIndex) = *(first + parent);
        holeIndex = parent;
        parent = (holeIndex - 1) / 2;
    }
    *(first + holeIndex) = value;  // holeIndex == 0 || *(first + parent) >= value, 也就是holeIndex到达指定位置
}

// 该函数获取Distance类型
template <class RandomIterator, class Distance, class Compare>
void push_heap_d(RandomIterator first, RandomIterator last, Distance *, Compare comp) {
    push_heap_aux(first, static_cast<Distance>(last - first - 1), static_cast<Distance>(0), *(last - 1), comp);
}


template <class RandomIterator, class Compare>
void push_heap(RandomIterator first, RandomIterator last, Compare comp) {
    push_heap_d(first, last, difference_type(first), comp);
}

/*
*   对于pop_heap而言，是把[first, last)区间中的，最头部的值，放在last - 1，而last - 1的元素用value保存
*   调整根堆呢，length自然要比最初传进来的要少一个，并且下溯的过程，不与child比较谁大谁小，而是将两个child大的统统换上去
*   最后holdeIdex便到达了最底层，而最底层则是value，但是value还不在最终的位置 (因为下溯根本没有和value比较)，所以执行push_heap。
*/


// **********************************************************************************************
// pop_heap接收[first, last)区间，表示heap的首尾，将首部元素放在尾部，进行下溯操作，不考虑容器真正的弹出
// **********************************************************************************************

// adjust_heap 调整[first, first + len)中的holeIndex元素，其元素为value，但未实际覆盖，只是记录了
template <class RandomIterator, class T, class Distance>
void adjust_heap(RandomIterator first, Distance holeIndex, Distance len, T value) {
    // 执行下溯过程
    Distance topIndex = holeIndex;
    Distance rchild = holeIndex * 2 + 2;
    while(rchild < len) {
        // 左边更大一点
        if(*(first + rchild) < *(first + rchild - 1)) {
            --rchild;
        }
        *(first + holeIndex) = *(first + rchild);    // 把更大的元素上移，并不考虑跟value的比较
        holeIndex = rchild;
        rchild = rchild * 2 + 2;
    }
    if(rchild == len) {
        // 无右节点，但有左结点
        *(first + holeIndex) = *(first + (rchild - 1));
        holeIndex = rchild - 1;
    }

    // 再次执行上溯
    // 相当于此时value在holeIndex的位置，但是有可能会出现value比父母大的情况，因为上面的操作，只管上移，没有跟value比较
    push_heap_aux(first, holeIndex, topIndex, value);   
}

// 调整[first, last)区间的heap，并且把头部的元素放在
template <class RandomIterator, class T, class Distance>
void pop_heap_aux(RandomIterator first, RandomIterator last, RandomIterator result, T value, Distance*) {
    *result = *first;   // 把头部最大的值放在result上，且result原有的数据value保存了，然后对长度减一的区间adjust
    adjust_heap(first, static_cast<Distance>(0), last - first, value);
}

template <class RandomIterator>
void pop_heap(RandomIterator first, RandomIterator last) {
    pop_heap_aux(first, last - 1, last - 1, *(last - 1), difference_type(first)); // 将尾部的被替代的元素保存传进去
}

// comp

template <class RandomIterator, class T, class Distance, class Compare>
void adjust_heap(RandomIterator first, Distance holeIndex, Distance len, T value, Compare comp) {
    // 执行下溯过程
    Distance topIndex = holeIndex;
    Distance rchild = holeIndex * 2 + 2;
    while(rchild < len) {
        // 左边更大一点
        if(comp(*(first + rchild), *(first + rchild - 1))) {
            --rchild;
        }
        *(first + holeIndex) = *(first + rchild);
        holeIndex = rchild;
        rchild = rchild * 2 + 2;
    }
    if(rchild == len) {
        // 无右节点，但有左结点
        *(first + holeIndex) = *(first + rchild - 1);
        holeIndex = rchild - 1;
    }

    push_heap_aux(first, holeIndex, topIndex, value, comp);   
}

// 调整[first, last)区间的heap，并且把头部的元素放在
template <class RandomIterator, class T, class Distance, class Compare>
void pop_heap_aux(RandomIterator first, RandomIterator last, RandomIterator result, T value, Distance*, Compare comp) {
    *result = *first;   // 把头部最大的值放在result上，且result原有的数据value保存了，然后对长度减一的区间adjust
    adjust_heap(first, static_cast<Distance>(0), last -first, value, comp);
}

template <class RandomIterator, class Compare>
void pop_heap(RandomIterator first, RandomIterator last, Compare comp) {
    pop_heap_aux(first, last - 1, last - 1, *(last - 1), difference_type(first), comp); // 将尾部的被替代的元素保存传进去
}


// **************************************************************************************
// sort_heap接收[first, last)区间，表示heap的首尾，不断pop_heap，并且len - 1，这样就完成了排序
// **************************************************************************************
template <class RandomIterator>
void sort_heap(RandomIterator first, RandomIterator last) {
    while(last - first > 1) {
        pop_heap(first, last--);
    }
}

template <class RandomIterator, class Compare>
void sort_heap(RandomIterator first, RandomIterator last, Compare comp) {
    while(last - first > 1) {
        pop_heap(first, last--, comp);
    }
}


// ***************************************************************
// meke_heap接收[first, last)区间，表示heap的首尾，将容器变为一个heap
// ***************************************************************
template <class RandomIterator, class Distance>
void make_heap_aux(RandomIterator first, RandomIterator last, Distance*) {
    if(last - first < 2)    return;

    Distance len = last - first;
    Distance holeIndex = (len - 2) / 2;     // 第一个非叶节点下标
    while(holeIndex >= 0) {
        adjust_heap(first, holeIndex, len, *(first + holeIndex));
        --holeIndex;
    }
}


template <class RandomIterator>
void make_heap(RandomIterator first, RandomIterator last) {
    make_heap_aux(first, last, difference_type(first));
}

// Compare
template <class RandomIterator, class Distance, class Compare>
void make_heap_aux(RandomIterator first, RandomIterator last, Distance*, Compare comp) {
    if(last - first < 2)    return;

    Distance len = last - first;
    Distance holeIndex = (len - 2) / 2;     // 第一个非叶节点下标
    while(holeIndex >= 0) {
        adjust_heap(first, holeIndex, len, *(first + holeIndex), comp);
        --holeIndex;
    }
}


template <class RandomIterator, class Compare>
void make_heap(RandomIterator first, RandomIterator last, Compare comp) {
    make_heap_aux(first, last, difference_type(first), comp);
}

} // !namespace mystl

#endif