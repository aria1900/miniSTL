#ifndef __ALGOBASE_h__
#define __ALGOBASE_h__

#include <vector>
#include <iostream>

#include "../MySTL/algobase.h"

namespace algobase_test {
void print(mystl::input_iterator_tag) {
    std::cout << "input_iterator" << std::endl;
}

void print(mystl::random_access_iterator_tag) {
    std::cout << "random_iterator" << std::endl;
}

// std定义的东西，只能反射到std上面，名字相同也不行
void print(std::random_access_iterator_tag) {
    std::cout << "std random_iterator" << std::endl;
}

void test() {
    std::cout << "--------------algobase_test-------------" << std::endl;
    std::cout << "copy:" << std::endl;
    int nums1[5] = {1, 2, 3, 4, 5};;
    int nums2[5];
    mystl::copy(nums1, nums1 + 5, nums2);
    for(int num : nums2) {
        std::cout << num << " ";
    }
    std::cout << std::endl;

    std::cout << "int* category : ";
    int * ptr;
    print(mystl::iterator_category(ptr));

    std::vector<int> v1;
    std::cout << "vector iterator category : ";
    print(mystl::iterator_category(v1.begin()));

    std::cout << "move:" << std::endl;
    int nums3[5];
    mystl::move(nums1, nums1 + 5, nums3);
    for(int num : nums3) {
        std::cout << num << " ";
    }
    std::cout << std::endl;
}

}

#endif 