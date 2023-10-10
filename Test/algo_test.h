#ifndef __ALGO_TEST_H__
#define __ALGO_TEST_H__

#include "test.h"
#include "../MySTL/algo.h"
#include "../MySTL/vector.h"
#include "../MySTL/iterator.h"

#include <iostream>
#include <algorithm>
#include <chrono>

using namespace std;
using namespace std::chrono;

namespace algo_test {

template <class Container>
bool check(Container& con) {
    auto it = con.begin();
    auto pre = it;
    ++it;
    for(; it != con.end() ; ++it) {
        if(*it < *pre) return false;
        pre = it;
    }
    return true;
}


void test() {
    // unique
    mystl::vector<int> nums = {1,1,2,2,2,3,4,4};
    auto finish = mystl::unique(nums.begin(), nums.end());
    for(auto start = nums.begin() ; start != finish ; ++start) {
        std::cout << *start << " ";
    }
    std::cout << std::endl;
    COUT(nums);
    std::cout << std::endl;

    // next_permutation
    mystl::vector<int> perm = {1,2,3,4,5};
    while(true) {
        COUT(perm);
        if(!mystl::next_permutation(perm.begin(), perm.end())) {
            break;      // 若为假则break，到达最后一个排列了
        }
    }

    // merge
    mystl::vector<int> vec1 = {0,1,1,4,6,7};
    mystl::vector<int> vec2 = {3,5,6,8};
    mystl::vector<int> result;
    mystl::merge(vec1.begin(), vec1.end(), vec2.begin(), vec2.end(), mystl::inserter(result, result.begin())); // 按顺序从begin插入
    COUT(result);

    // sort
    mystl::vector<int> v1 = {3,2,5,7,1,0,4,9};
    mystl::sort(v1.begin(), v1.end(), mystl::greater<int>());
    COUT(v1);

    std::cout << "<-----------------------------Sort Performance Testing---------------------------> \n";
    
    const int N = 100000000;  // 1e
    const int M = 10000000;   //1000万
    mystl::vector<int> nums1;
    mystl::vector<int> nums2;
    nums1.reserve(N);
    nums2.reserve(N);
    // 输入数据
    srand(time(0));
    for(int i = N ; i >=0 ; i--) {
        int value = rand();
        nums1.push_back(value);
        nums2.push_back(value);
    }

    auto start = high_resolution_clock::now();
    std::sort(nums1.begin(), nums1.end());
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    std::cout << "std::sort sort " << N <<" elements use the time :" << duration.count() << " ms" << std::endl;
    std::cout << "The nums1 is sorted ? " << (check(nums1) ? "Yes" : "No") << std::endl;

    start = high_resolution_clock::now();
    mystl::sort(nums2.begin(), nums2.end());
    end = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(end - start);
    std::cout << "mystl::sort sort " << N <<" elements use the time :" << duration.count() << " ms" << std::endl;
    std::cout << "The nums2 is sorted ? " << (check(nums2) ? "Yes" : "No") << std::endl;
    std::cout << std::endl;
}


}

#endif