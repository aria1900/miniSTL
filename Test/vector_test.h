#ifndef __VECTOR_TEST_H__
#define __VECTOR_TEST_H__

#include <iostream>
#include <vector>
#include <chrono>

#include "../MySTL/vector.h"
#include "test.h"

using namespace std::chrono;

namespace vector_test{

void test() {
    std::cout << "--------------------------vector test-----------------------" << std::endl;
    int a[] = { 1,2,3,4,5 };
    mystl::vector<int> v1;
    mystl::vector<int> v2(10);
    mystl::vector<int> v3(10, 1);
    mystl::vector<int> v4(a, a + 5);
    mystl::vector<int> v5(v2);
    mystl::vector<int> v6(std::move(v2));
    mystl::vector<int> v7{ 1,2,3,4,5,6,7,8,9 };
    mystl::vector<int> v8, v9, v10;
    v8 = v3;
    v9 = std::move(v3);
    v10 = { 1,2,3,4,5,6,7,8,9 };

    // 改变容器内元素的操作
    FUN_AFTER(v1, v1.assign(8, 8));
    FUN_AFTER(v1, v1.assign(a, a + 5));
    FUN_AFTER(v1, v1.push_back(6));
    FUN_AFTER(v1, v1.insert(v1.end(), 7));
    FUN_AFTER(v1, v1.insert(v1.begin() + 3, 2, 3));
    FUN_AFTER(v1, v1.insert(v1.begin(), a, a + 5));
    FUN_AFTER(v1, v1.pop_back());
    FUN_AFTER(v1, v1.erase(v1.begin()));
    FUN_AFTER(v1, v1.erase(v1.begin(), v1.begin() + 2));
    FUN_AFTER(v1, v1.swap(v4));

    // 元素访问的操作
    FUN_VALUE(*v1.begin());
    FUN_VALUE(*(v1.end() - 1));
    FUN_VALUE(*v1.rbegin());
    FUN_VALUE(*(v1.rend() - 1));
    FUN_VALUE(v1.front());
    FUN_VALUE(v1.back());
    FUN_VALUE(v1[0]);
    FUN_VALUE(v1.at(1));
    int* p = v1.data();
    *p = 10;
    *++p = 20;
    p[1] = 30;
    std::cout << "After change v1.data() :" << "\n";
    COUT(v1);

    std::cout<< std::endl;
    // 有关容量的操作
    std::cout << std::boolalpha;
    FUN_VALUE(v1.empty());
    std::cout << std::noboolalpha;
    FUN_VALUE(v1.size());
    FUN_VALUE(v1.max_size());
    FUN_VALUE(v1.capacity());
    FUN_AFTER(v1, v1.resize(10));
    FUN_VALUE(v1.size());
    FUN_VALUE(v1.capacity());
    FUN_AFTER(v1, v1.shrink_to_fit());
    FUN_VALUE(v1.size());
    FUN_VALUE(v1.capacity());
    FUN_AFTER(v1, v1.resize(6, 6));
    FUN_VALUE(v1.size());
    FUN_VALUE(v1.capacity());
    FUN_AFTER(v1, v1.shrink_to_fit());
    FUN_VALUE(v1.size());
    FUN_VALUE(v1.capacity());
    FUN_AFTER(v1, v1.clear());
    FUN_VALUE(v1.size());
    FUN_VALUE(v1.capacity());
    FUN_AFTER(v1, v1.reserve(5));
    FUN_VALUE(v1.size());
    FUN_VALUE(v1.capacity());
    FUN_AFTER(v1, v1.reserve(20));
    FUN_VALUE(v1.size());
    FUN_VALUE(v1.capacity());
    FUN_AFTER(v1, v1.shrink_to_fit());
    FUN_VALUE(v1.size());
    FUN_VALUE(v1.capacity());
    std::cout << std::endl;

    std::cout << "Performance Testing \n";
    std::vector<int> stdVec;
    const int N = 100000000; //一亿个数的push_back
    const int M = 300000; //50万的insert
    
    auto start = high_resolution_clock::now();
    for(int i = 0 ; i < N ; i++) {
        stdVec.push_back(i);
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    std::cout << "std::vector push_back 100,000,000 elements use the time :" << duration.count() << " ms" << std::endl;
    std::cout << "stdVec size : " << stdVec.size() << " stdVec capacity : " << stdVec.capacity() << std::endl;

    start = high_resolution_clock::now();
    for(int i = 0 ; i < N ; i++) {
        v1.push_back(i);
    }
    end = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(end - start);
    std::cout << "mystl::vector push_back 100,000,000 elements use the time :" << duration.count() << " ms" << std::endl;
    std::cout << "v1 size : " << v1.size() << " v1 capacity : " << v1.capacity() << std::endl;

    v1.clear();
    v1.shrink_to_fit();
    stdVec.clear();
    stdVec.shrink_to_fit();
    
    std::cout << "\n";
    std::cout << "v1 and stdVec after clear() and shrink_to_fit()" << std::endl;
    std::cout << "\n";

    // insert
    start = high_resolution_clock::now();
    for(int i = 0 ; i < M ; i++) {
        if(i == 0)  stdVec.push_back(i);
        else    stdVec.insert(stdVec.begin() + 1, i);
    }
    end = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(end - start);
    std::cout << "std::vector insert (pos : 1) 300,000 elements use the time :" << duration.count() << " ms" << std::endl;
    std::cout << "stdVec size : " << stdVec.size() << " stdVec capacity : " << stdVec.capacity() << std::endl;

    start = high_resolution_clock::now();
    for(int i = 0 ; i < M ; i++) {
        if(i == 0)  v1.push_back(i);
        else    v1.insert(v1.begin() + 1, i);
    }
    end = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(end - start);
    std::cout << "mystl::vector insert (pos : 1) 300,000 elements use the time :" << duration.count() << " ms" << std::endl;
    std::cout << "v1 size : " << v1.size() << " v1 capacity : " << v1.capacity() << std::endl;

    std::cout << std::endl;
}

} // namespace vector_test
#endif