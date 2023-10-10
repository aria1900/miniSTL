#ifndef __UNORDERED_MAP_TEST_H__
#define __UNORDERED_MAP_TEST_H__

#include "../MySTL/unordered_map.h"
#include "test.h"

#include <iostream>
#include <unordered_map>


namespace unordered_map_test {

//#define PAIR    mystl::pair<int,int>

/* // map 的遍历输出
#define MAP_COUT(m) do {    \
    std::string m_name = #m; \
    std::cout << m_name << " :"; \
    for (auto it : m)    std::cout << "<" << it.first << "," << it.second << ">"; \
    std::cout << std::endl; \
} while(0)

// map 的函数操作
#define MAP_FUN_AFTER(con, fun) do { \
    std::string str = #fun; \
    std::cout << "After " << str << " :" << std::endl; \
    fun; \
    MAP_COUT(con); \
} while(0)

// map 的函数值
#define MAP_VALUE(fun) do { \
    std::string str = #fun; \
    auto it = fun; \
    std::cout <<  str << " : <" << it.first << "," << it.second << ">\n"; \
} while(0) */


void test() {
    std::cout << "--------------------------unordered_map test-----------------------" << std::endl;
    mystl::vector<PAIR> v;
    for (int i = 0; i < 5; ++i)
        v.push_back(PAIR(5 - i, 5 - i));
    mystl::unordered_map<int, int> um1;
    mystl::unordered_map<int, int> um2(520);
    mystl::unordered_map<int, int> um3(520, mystl::hash<int>());
    mystl::unordered_map<int, int> um4(520, mystl::hash<int>(), mystl::equal_to<int>());
    mystl::unordered_map<int, int> um5(v.begin(), v.end());
    mystl::unordered_map<int, int> um6(v.begin(), v.end(), 100);
    mystl::unordered_map<int, int> um7(v.begin(), v.end(), 100, mystl::hash<int>());
    mystl::unordered_map<int, int> um8(v.begin(), v.end(), 100, mystl::hash<int>(), mystl::equal_to<int>());
    mystl::unordered_map<int, int> um9(um5);
    mystl::unordered_map<int, int> um10(std::move(um5));
    mystl::unordered_map<int, int> um11;
    mystl::unordered_map<int, int> um12;
    um12 = std::move(um6);
    mystl::unordered_map<int, int> um13{ PAIR(1,1),PAIR(2,3),PAIR(3,3) };
    mystl::unordered_map<int, int> um14;
    um14 = { PAIR(1,1),PAIR(2,3),PAIR(3,3) };

    MAP_FUN_AFTER(um1, um1.insert(PAIR(2, 2)));
    MAP_FUN_AFTER(um1, um1.insert(PAIR(3, 3)));
    MAP_FUN_AFTER(um1, um1.insert(v.begin(), v.end()));
    MAP_FUN_AFTER(um1, um1.erase(um1.begin()));
    MAP_FUN_AFTER(um1, um1.erase(um1.begin(), um1.find(3)));
    MAP_FUN_AFTER(um1, um1.erase(1));
    std::cout << std::boolalpha;
    FUN_VALUE(um1.empty());
    std::cout << std::noboolalpha;
    FUN_VALUE(um1.size());
    FUN_VALUE(um1.bucket_count());
    FUN_VALUE(um1.max_bucket_count());
    FUN_VALUE(um1.elems_in_bucket(1));
    FUN_VALUE(um1.bucket_count());
    MAP_FUN_AFTER(um1, um1.clear());
    MAP_FUN_AFTER(um1, um1.swap(um7));
    MAP_VALUE(*um1.begin());
    FUN_VALUE(um1.at(1));
    FUN_VALUE(um1[1]);
    std::cout << std::boolalpha;
    FUN_VALUE(um1.empty());
    std::cout << std::noboolalpha;
    FUN_VALUE(um1.size());
    FUN_VALUE(um1.max_size());
    FUN_VALUE(um1.bucket_count());
    FUN_VALUE(um1.max_bucket_count());
    FUN_VALUE(um1.elems_in_bucket(1));
    FUN_VALUE(um1.bucket_count());
    FUN_VALUE(um1.size());
    FUN_VALUE(um1.bucket_count());
    FUN_VALUE(um1.count(1));
    MAP_VALUE(*um1.find(3));
    auto first = *um1.equal_range(3).first;
    auto second = *um1.equal_range(3).second;
    std::cout << " um1.equal_range(3) : from <" << first.first << ", " << first.second
        << "> to <" << second.first << ", " << second.second << ">" << std::endl;

    std::cout << "<----------------------Performance Testing-------------------------> \n";
    std::unordered_map<int,int> stdUm;
    mystl::unordered_map<int,int> mystlUm;
    const int N = 100000000;  // 1e
    const int M = 10000000;   //1000万
    
    auto start = high_resolution_clock::now();
    for(int i = M ; i >=0 ; i--) {
        stdUm.insert(std::pair(i,i));
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    std::cout << "std::unordered_map insert " << M <<" elements use the time :" << duration.count() << " ms" << std::endl;

    start = high_resolution_clock::now();
    for(int i = M ; i >=0 ; i--) {
        mystlUm.insert(PAIR(i,i));
    }
    end = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(end - start);
    std::cout << "mystl::unordered_map insert " << M <<" elements use the time :" << duration.count() << " ms" << std::endl;
    std::cout << std::endl;
}

}

#endif