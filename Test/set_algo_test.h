#ifndef __SET_ALGO_TEST_H__
#define __SET_ALGO_TEST_H__

#include "../MySTL/vector.h"
#include "../MySTL/set_algo.h"
#include "../MySTL/iterator.h"
#include "test.h"

namespace set_algo_test {

void test() {
    std::cout << "----------------------set algo test-------------------------- \n"; 
    mystl::vector<int> s1 = {1,3,5,7,9,11};
    mystl::vector<int> s2 = {1,1,2,3,5,8,13};

    mystl::vector<int> result;      // 采用back_inserter来完成答案的输出

    mystl::set_union(s1.begin(), s1.end(), s2.begin(), s2.end(), back_inserter(result));
    std::cout << "s1 union s2 : ";
    COUT(result);
    result.clear();

    mystl::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), back_inserter(result));
    std::cout << "s1 intersection s2 : ";
    COUT(result);
    result.clear();

    mystl::set_difference(s1.begin(), s1.end(), s2.begin(), s2.end(), back_inserter(result));
    std::cout << "s1 difference s2 : ";
    COUT(result);
    result.clear();

    mystl::set_difference(s2.begin(), s2.end(), s1.begin(), s1.end(), back_inserter(result));
    std::cout << "s2 difference s1 : ";
    COUT(result);
    result.clear();

    mystl::set_symmetric_difference(s1.begin(), s1.end(), s2.begin(), s2.end(), back_inserter(result));
    std::cout << "s1 symmetric_difference s2 : ";
    COUT(result);
    result.clear();

    std::cout << std::endl;
}

}

#endif