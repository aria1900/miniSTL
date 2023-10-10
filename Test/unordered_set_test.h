#ifndef __UNORDERED_SET_TEST_H__
#define __UNORDERED_SET_TEST_H__

#include <iostream>
#include <unordered_set>
#include <chrono>

#include "../MySTL/unordered_set.h"
#include "test.h"

using namespace std::chrono;

namespace unordered_set_test {

void test() {
    std::cout << "--------------------------unordered_set test-----------------------" << std::endl;
    int a[] = { 5,4,3,2,1 };
    mystl::unordered_set<int> us1;
    mystl::unordered_set<int> us2(520);
    mystl::unordered_set<int> us3(520, mystl::hash<int>());
    mystl::unordered_set<int> us4(520, mystl::hash<int>(), mystl::equal_to<int>());
    mystl::unordered_set<int> us5(a, a + 5);
    mystl::unordered_set<int> us6(a, a + 5, 100);
    mystl::unordered_set<int> us7(a, a + 5, 100, mystl::hash<int>());
    mystl::unordered_set<int> us8(a, a + 5, 100, mystl::hash<int>(), mystl::equal_to<int>());
    mystl::unordered_set<int> us9(us5);
    mystl::unordered_set<int> us10(std::move(us5));
    mystl::unordered_set<int> us11;
    us11 = us6;
    mystl::unordered_set<int> us12;
    us12 = std::move(us6);
    mystl::unordered_set<int> us13{ 1,2,3,4,5 };
    mystl::unordered_set<int> us14;
    us13 = { 1,2,3,4,5 };


    FUN_AFTER(us1, us1.insert(5));
    FUN_AFTER(us1, us1.insert(a, a + 5));
    FUN_AFTER(us1, us1.erase(us1.begin()));
    FUN_AFTER(us1, us1.erase(us1.begin(), us1.find(3)));
    FUN_AFTER(us1, us1.erase(1));
    std::cout << std::boolalpha;
    FUN_VALUE(us1.empty());
    std::cout << std::noboolalpha;
    FUN_VALUE(us1.size());
    FUN_VALUE(us1.bucket_count());
    FUN_VALUE(us1.max_bucket_count());
    FUN_VALUE(us1.elems_in_bucket(1));
    FUN_AFTER(us1, us1.clear());
    FUN_AFTER(us1, us1.swap(us7));
    FUN_VALUE(*us1.begin());
    std::cout << std::boolalpha;
    FUN_VALUE(us1.empty());
    std::cout << std::noboolalpha;
    FUN_VALUE(us1.size());
    FUN_VALUE(us1.max_size());
    FUN_VALUE(us1.bucket_count());
    FUN_VALUE(*us1.begin());
    FUN_VALUE(us1.size());
    FUN_VALUE(us1.bucket_count());
    FUN_VALUE(us1.bucket_count());
    FUN_VALUE(us1.count(1));
    FUN_VALUE(*us1.find(3));
    auto first = *us1.equal_range(3).first;
    auto second = *us1.equal_range(3).second;
    std::cout << "us1.equal_range(3) : from " << first << " to " << second << std::endl;

    std::cout << std::endl;
    std::cout << "****************Performance Testing******************* \n";
    std::unordered_set<int> stdUs;
    mystl::unordered_set<int> mystlUs;
    const int N = 100000000;  // 1e
    const int M = 10000000;   //1000万
    
    auto start = high_resolution_clock::now();
    for(int i = M ; i >= 0 ; i--) {
        stdUs.insert(i);
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    std::cout << "std::unordered_set insert " << M <<" elements use the time :" << duration.count() << " ms" << std::endl;

    start = high_resolution_clock::now();
    for(int i = M ; i >= 0 ; i--) {
        mystlUs.insert(i);
    }
    end = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(end - start);
    std::cout << "mystl::unordered_set insert " << M <<" elements use the time :" << duration.count() << " ms" << std::endl;
    std::cout << std::endl;
}

}


#endif