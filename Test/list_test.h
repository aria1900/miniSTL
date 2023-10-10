#ifndef __LIST_TEST_H__
#define __LIST_TEST_H__

#include <iostream>
#include <algorithm>
#include <list>

#include "../MySTL/list.h"
#include "test.h"

namespace list_test{

template <class Container>
bool check(Container& con) {
    for(auto it = con.begin() ; it != --con.end() ; ++it) {
        auto next = it;
        ++next;
        if(*next < *it) return false;
    }
    return true;
}

void test() {
    std::cout << "--------------------------list test-----------------------" << std::endl;
    int a[] = { 1,2,3,4,5 };
    mystl::list<int> l1;
    mystl::list<int> l2(5);
    mystl::list<int> l3(5, 1);
    mystl::list<int> l4(a, a + 5);
    mystl::list<int> l5(l2);
    mystl::list<int> l6(std::move(l2));
    mystl::list<int> l7{ 1,2,3,4,5,6,7,8,9 };
    mystl::list<int> l8;
    l8 = l3;
    mystl::list<int> l9;
    l9 = std::move(l3);
    mystl::list<int> l10;
    l10 = { 1, 2, 2, 3, 5, 6, 7, 8, 9 };

    FUN_AFTER(l1, l1.assign(8, 8));
    FUN_AFTER(l1, l1.assign(a, a + 5));
    FUN_AFTER(l1, l1.assign({ 1,2,3,4,5,6 }));
    FUN_AFTER(l1, l1.insert(l1.end(), 6));
    FUN_AFTER(l1, l1.insert(l1.end(), 2, 7));
    FUN_AFTER(l1, l1.insert(l1.begin(), a, a + 5));
    FUN_AFTER(l1, l1.push_back(2));
    FUN_AFTER(l1, l1.push_front(1));
    FUN_VALUE(l1.size());
    FUN_AFTER(l1, l1.pop_front());
    FUN_AFTER(l1, l1.pop_back());
    FUN_AFTER(l1, l1.erase(l1.begin()));
    FUN_AFTER(l1, l1.erase(l1.begin(), l1.end()));
    FUN_VALUE(l1.size());
    FUN_AFTER(l1, l1.resize(10));
    FUN_AFTER(l1, l1.resize(5, 1));
    FUN_AFTER(l1, l1.resize(8, 2));
    FUN_VALUE(l1.size());
    FUN_AFTER(l1, l1.splice(l1.end(), l4));
    FUN_AFTER(l1, l1.splice(l1.begin(), l5, l5.begin()));
    FUN_AFTER(l1, l1.splice(l1.end(), l6, l6.begin(), ++l6.begin()));
    FUN_VALUE(l1.size());
    FUN_AFTER(l1, l1.remove(0));
    FUN_VALUE(l1.size());
    FUN_AFTER(l1, l1.assign({ 9,5,3,3,7,1,3,2,2,0,10 }));
    FUN_VALUE(l1.size());
    FUN_AFTER(l1, l1.sort());
    FUN_AFTER(l1, l1.unique());
    FUN_AFTER(l1, l1.unique([&](int a, int b) {return b == a - 1; }));
    FUN_AFTER(l1, l1.merge(l7));
    FUN_AFTER(l1, l1.sort(std::greater<int>()));
    FUN_AFTER(l1, l1.merge(l8, std::greater<int>()));
    FUN_AFTER(l1, l1.reverse());
    FUN_AFTER(l1, l1.clear());
    FUN_AFTER(l1, l1.swap(l9));
    FUN_VALUE(*l1.begin());
    FUN_VALUE(*l1.rbegin());
    FUN_VALUE(l1.front());
    FUN_VALUE(l1.back());
    std::cout << std::boolalpha;
    FUN_VALUE(l1.empty());
    std::cout << std::noboolalpha;
    FUN_VALUE(l1.size());
    FUN_VALUE(l1.max_size());
    mystl::list<int> l11{ 9,5,3,3,7,1,3,2,2,0,10 };
    FUN_AFTER(l11, l11.sort());
    std::cout << std::endl;

    std::cout << "Performance Testing \n";
    std::list<int> stdList;
    mystl::list<int> mystlList;
    const int N = 100000000;  // 1e
    const int M = 10000000;   // 1000w

    srand(time(0));

    // push_front
    auto start = high_resolution_clock::now();
    for(int i = 0 ; i < M ; i++) {
        int number = rand() % M;
        stdList.insert(stdList.begin(), number);
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    std::cout << "std::list push_front " << M << " random elements use the time :" << duration.count() << " ms" << std::endl;

    start = high_resolution_clock::now();
    for(int i = 0 ; i < M ; i++) {
        int number = rand() % M;
        mystlList.insert(mystlList.begin(), number);
    }
    end = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(end - start);
    std::cout << "mystl::list push_front " << M << " random elements use the time :" << duration.count() << " ms" << std::endl;

    // sort

    std::cout << "sort begin" << std::endl;
    start = high_resolution_clock::now();
    stdList.sort();
    end = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(end - start);
    std::cout << "std::list sort " << M << " random elements use the time :" << duration.count() << " ms" << std::endl;

    start = high_resolution_clock::now();
    mystlList.sort();
    end = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(end - start);
    std::cout << "mystl::list sort " << M << " random elements use the time :" << duration.count() << " ms" << std::endl;

    std::cout << "stdList is sorted :" << check(stdList) << std::endl;
    std::cout << "mystlList is sorted :" << check(mystlList) << std::endl;
}

} // namespace list_test

#endif