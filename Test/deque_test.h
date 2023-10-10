#ifndef __DEQUE_TEST_H__
#define __DEQUE_TEST_H__

#include <iostream>
#include <algorithm>
#include <deque>
#include <chrono>

#include "../MySTL/deque.h"
#include "test.h"

using namespace std::chrono;

namespace deque_test {

void test() {
    std::cout << "--------------------------deque test-----------------------" << std::endl;
    int a[] = { 1,2,3,4,5 };
    mystl::deque<int> d1;
    mystl::deque<int> d2(5);
    mystl::deque<int> d3(5, 1);
    mystl::deque<int> d4(a, a + 5);
    mystl::deque<int> d5(d2);
    mystl::deque<int> d6(std::move(d2));
    mystl::deque<int> d7;
    d7 = d3;
    mystl::deque<int> d8;
    d8 = std::move(d3);
    mystl::deque<int> d9{ 1,2,3,4,5,6,7,8,9 };
    mystl::deque<int> d10;
    d10 = { 1,2,3,4,5,6,7,8,9 };

    //FUN_AFTER(d1, d1.insert(d1.end(), 6));
    FUN_AFTER(d1, d1.assign(5, 1));
    FUN_AFTER(d1, d1.assign(8, 8));
    FUN_AFTER(d1, d1.assign(a, a + 5));
    FUN_AFTER(d1, d1.assign({ 1,2,3,4,5 }));
    FUN_AFTER(d1, d1.insert(d1.end(), 6));
    FUN_AFTER(d1, d1.insert(d1.end() - 1, 2, 7));
    FUN_AFTER(d1, d1.insert(d1.begin(), a, a + 5));
    FUN_AFTER(d1, d1.erase(d1.begin()));
    FUN_AFTER(d1, d1.erase(d1.begin(), d1.begin() + 4));
    FUN_AFTER(d1, d1.push_front(1));
    FUN_AFTER(d1, d1.push_back(2));
    FUN_AFTER(d1, d1.pop_back());
    FUN_AFTER(d1, d1.pop_front());
    //FUN_AFTER(d1, d1.shrink_to_fit());
    FUN_AFTER(d1, d1.resize(5));
    FUN_AFTER(d1, d1.resize(8, 8));
    FUN_AFTER(d1, d1.clear());
    //FUN_AFTER(d1, d1.shrink_to_fit());
    FUN_AFTER(d1, d1.swap(d4));
    COUT(d4);
    FUN_VALUE(*(d1.begin()));
    FUN_VALUE(*(d1.end() - 1));
    FUN_VALUE(*(d1.rbegin()));
    FUN_VALUE(*(d1.rend() - 1));
    FUN_VALUE(d1.front());
    FUN_VALUE(d1.back());
    FUN_VALUE(d1.at(1));
    FUN_VALUE(d1[2]);
    std::cout << std::boolalpha;
    FUN_VALUE(d1.empty());
    std::cout << std::noboolalpha;
    FUN_VALUE(d1.size());
    FUN_VALUE(d1.max_size());

    std::cout << std::endl << "------- performance test ----------" << std::endl;
    const int M = 100000000; //1e

    std::deque<int> stdDeque;
    mystl::deque<int> mystlDeque;

    // push_back

    auto start = high_resolution_clock::now();
    for(int i = 0 ; i < M ; i ++) {
        stdDeque.push_back(i);
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    std::cout << "std::deque push_back 1*10^8 elements use the time :" << duration.count() << " ms. " << "size() : " << stdDeque.size() << std::endl;

    start = high_resolution_clock::now();
    for(int i = 0 ; i < M ; i ++) {
        mystlDeque.push_back(i);
    }
    end = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(end - start);
    std::cout << "mystl::deque push_back 1*10^8 elements use the time :" << duration.count() << " ms. " << "size() : " << mystlDeque.size() <<std::endl;
    
    mystlDeque.clear();
    stdDeque.clear();

    // push_front

    start = high_resolution_clock::now();
    for(int i = 0 ; i < M ; i ++) {
        stdDeque.push_front(i);
    }
    end = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(end - start);
    std::cout << "std::deque push_front 1*10^8 elements use the time :" << duration.count() << " ms. " << "size() : " << stdDeque.size() <<std::endl;

    start = high_resolution_clock::now();
    for(int i = 0 ; i < M ; i ++) {
        mystlDeque.push_back(i);
    }
    end = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(end - start);
    std::cout << "mystl::deque push_front 1*10^8 elements use the time :" << duration.count() << " ms. " << "size() : " << mystlDeque.size() <<std::endl;
    std::cout << std::endl;
    //mystlDeque.debugFunc();
}

}

#endif