#ifndef __SET_TEST_H__
#define __SET_TEST_H__

#include <iostream>
#include <set>
#include <chrono>

#include "../MySTL/set.h"
#include "test.h"

using namespace std::chrono;

namespace set_test {

void test() {
    std::cout << "--------------------------set test-----------------------" << std::endl;
    int a[] = { 5,4,3,2,1 };
    mystl::set<int> s1;
    mystl::set<int, mystl::greater<int>> s2;
    mystl::set<int> s3(a, a + 5);
    mystl::set<int> s4(a, a + 5);
    mystl::set<int> s5(s3);
    mystl::set<int> s6(std::move(s3));
    mystl::set<int> s7;
    s7 = s4;
    mystl::set<int> s8;
    s8 = std::move(s4);
    mystl::set<int> s9{ 1,2,3,4,5 };
    mystl::set<int> s10;
    s10 = { 1,2,3,4,5 };

    for (int i = 5; i > 0; --i)
    {
        FUN_AFTER(s1, s1.insert(i));
    }
    FUN_AFTER(s1, s1.insert(s1.begin(), 0));
    FUN_AFTER(s1, s1.erase(s1.begin()));
    FUN_AFTER(s1, s1.erase(0));
    FUN_AFTER(s1, s1.erase(1));
    FUN_AFTER(s1, s1.erase(s1.begin(), s1.end()));
    for (int i = 0; i < 5; ++i)
    {
        FUN_AFTER(s1, s1.insert(i));
    }
    FUN_AFTER(s1, s1.insert(a, a + 5));
    FUN_AFTER(s1, s1.insert(5));
    FUN_AFTER(s1, s1.insert(s1.end(), 5));
    FUN_VALUE(s1.count(5));
    FUN_VALUE(*s1.find(3));
    FUN_VALUE(*s1.lower_bound(3));
    FUN_VALUE(*s1.upper_bound(3));
    auto first = *s1.equal_range(3).first;
    auto second = *s1.equal_range(3).second;
    std::cout << " s1.equal_range(3) : from " << first << " to " << second << std::endl;
    FUN_AFTER(s1, s1.erase(s1.begin()));
    FUN_AFTER(s1, s1.erase(1));
    FUN_AFTER(s1, s1.erase(s1.begin(), s1.find(3)));
    FUN_AFTER(s1, s1.clear());
    FUN_AFTER(s1, s1.swap(s5));
    FUN_VALUE(*s1.begin());
    FUN_VALUE(*s1.rbegin());
    std::cout << std::boolalpha;
    FUN_VALUE(s1.empty());
    std::cout << std::noboolalpha;
    FUN_VALUE(s1.size());
    FUN_VALUE(s1.max_size());


    std::cout << "<-----Performance Testing---------> \n";
    std::set<int> stdSet;
    mystl::set<int> mystlSet;
    const int N = 100000000;  // 1e
    const int M = 10000000;   //1000万
    
    auto start = high_resolution_clock::now();
    for(int i = M ; i >=0 ; i--) {
        stdSet.insert(i);
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    std::cout << "std::set insert " << M <<" elements use the time :" << duration.count() << " ms" << std::endl;

    start = high_resolution_clock::now();
    for(int i = M ; i >=0 ; i--) {
        mystlSet.insert(i);
    }
    end = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(end - start);
    std::cout << "mystl::set insert " << M <<" elements use the time :" << duration.count() << " ms" << std::endl;
    std::cout << std::endl;

    //COUT(stdSet);
    //COUT(mystlSet);
}

}

#endif