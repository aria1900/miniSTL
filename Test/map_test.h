#ifndef __MAP_TEST_H__
#define __MAP_TEST_H__

#include <iostream>
#include <initializer_list>
#include <map>

#include "../MySTL/map.h"
#include "../MySTL/vector.h"
#include "../MySTL/util.h"
#include "test.h"

namespace map_test {

// pair 的宏定义
#define PAIR    mystl::pair<int, int>

// map 的遍历输出
#define MAP_COUT(m) do { \
    std::string m_name = #m; \
    std::cout << m_name << " :"; \
    for (auto it : m)    std::cout << " <" << it.first << "," << it.second << ">"; \
    std::cout << std::endl; \
} while(0)  \

// map 的函数操作
#define MAP_FUN_AFTER(con, fun) do { \
    std::string str = #fun; \
    std::cout << "After " << str << " :" << std::endl; \
    fun; \
    MAP_COUT(con); \
} while(0)  \

// map 的函数值
#define MAP_VALUE(fun) do { \
    std::string str = #fun; \
    auto it = fun; \
    std::cout << str << " : <" << it.first << "," << it.second << ">\n"; \
} while(0)  \


void test() {
    std::cout << "--------------------------map test-----------------------" << std::endl;
    mystl::vector<PAIR> v;
    for (int i = 0; i < 5; ++i)
        v.push_back(PAIR(i, i));
    mystl::map<int, int> m1;
    mystl::map<int, int, mystl::greater<int>> m2;
    mystl::map<int, int> m3(v.begin(), v.end());
    mystl::map<int, int> m4(v.begin(), v.end());
    mystl::map<int, int> m5(m3);
    mystl::map<int, int> m6(std::move(m3));
    mystl::map<int, int> m7;
    m7 = m4;
    mystl::map<int, int> m8;
    m8 = std::move(m4);
    mystl::map<int, int> m9({ PAIR(1,1),PAIR(3,2),PAIR(2,3) });
    mystl::map<int, int> m10;
    m10 = { PAIR(1,1),PAIR(3,2),PAIR(2,3) };

    for (int i = 5; i > 0; --i)
    {
        MAP_FUN_AFTER(m1, m1.insert(PAIR(i,i)) );
    }
    MAP_FUN_AFTER(m1, m1.erase(m1.begin()));
    MAP_FUN_AFTER(m1, m1.erase(0));
    MAP_FUN_AFTER(m1, m1.erase(1));
    MAP_FUN_AFTER(m1, m1.erase(m1.begin(), m1.end()));
    for (int i = 0; i < 5; ++i)
    {
        MAP_FUN_AFTER(m1, m1.insert(PAIR(i, i)));
    }
    MAP_FUN_AFTER(m1, m1.insert(v.begin(), v.end()));
    MAP_FUN_AFTER(m1, m1.insert(m1.end(), PAIR(5, 5)));
    FUN_VALUE(m1.count(1));
    MAP_VALUE(*m1.find(3));
    MAP_VALUE(*m1.lower_bound(3));
    MAP_VALUE(*m1.upper_bound(2));
    auto first = *m1.equal_range(2).first;
    auto second = *m1.equal_range(2).second;
    std::cout << " m1.equal_range(2) : from <" << first.first << ", " << first.second
        << "> to <" << second.first << ", " << second.second << ">" << std::endl;
    MAP_FUN_AFTER(m1, m1.erase(m1.begin()));
    MAP_FUN_AFTER(m1, m1.erase(1));
    MAP_FUN_AFTER(m1, m1.erase(m1.begin(), m1.find(3)));
    MAP_FUN_AFTER(m1, m1.clear());
    MAP_FUN_AFTER(m1, m1.swap(m9));
    MAP_VALUE(*m1.begin());
    MAP_VALUE(*m1.rbegin());
    FUN_VALUE(m1[1]);
    MAP_FUN_AFTER(m1, m1[1] = 3);
    FUN_VALUE(m1.at(1));
    std::cout << std::boolalpha;
    FUN_VALUE(m1.empty());
    std::cout << std::noboolalpha;
    FUN_VALUE(m1.size());
    FUN_VALUE(m1.max_size());


    std::cout << "<-----Performance Testing---------> \n";
    std::map<int,int> stdMap;
    mystl::map<int,int> mystlMap;
    const int N = 100000000;  // 1e
    const int M = 10000000;   //1000万
    
    auto start = high_resolution_clock::now();
    for(int i = M ; i >=0 ; i--) {
        stdMap.insert({i,i});
    }
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(end - start);
    std::cout << "std::map insert " << M <<" elements use the time :" << duration.count() << " ms" << std::endl;

    start = high_resolution_clock::now();
    for(int i = M ; i >=0 ; i--) {
        mystlMap.insert(PAIR(i,i));
    }
    end = high_resolution_clock::now();
    duration = duration_cast<milliseconds>(end - start);
    std::cout << "mystl::map insert " << M <<" elements use the time :" << duration.count() << " ms" << std::endl;
    std::cout << std::endl;

    //MAP_COUT(mystlMap);

    
}

}

#endif