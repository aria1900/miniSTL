#ifndef __UNINITIALIZED_TEST_H__
#define __UNINITIALIZED_TEST_H__

#include <iostream>

#include "../MySTL/uninitialized.h"

namespace uninitialized_test{

class Person{
    private:
        int no;
    public:
        Person(int n) : no(n) {
            //std::cout << "Person ctor" << std::endl;
        }
        int get() {
            return no;
        }
};

void test() {
    std::cout << "------------uninitialized function test--------------" << std::endl;
    Person* nums1 = (Person*) malloc(10 * sizeof(Person)); // 10个位置
    Person* copyVec = (Person*) malloc(10 * sizeof(Person));
    mystl::uninitialized_fill(nums1, nums1 + 10, 10);
    mystl::uninitialized_copy(nums1, nums1 + 10, copyVec);
    std::cout << "nums1 element." << std::endl;
    for(int i = 0 ; i < 10 ; i++) {
        std::cout << nums1[i].get() << " ";
    }
    std::cout << std::endl;

    std::cout << "copyVec element." << std::endl;
    for(int i = 0 ; i < 10 ; i++) {
        std::cout << copyVec[i].get() << " ";
    }
    std::cout << std::endl;

    mystl::uninitialized_fill_n(nums1,10, 5);
    std::cout << "nums1 element." << std::endl;
    for(int i = 0 ; i < 10 ; i++) {
        std::cout << nums1[i].get() << " ";
    }
    std::cout << std::endl << std::endl;
}

}

#endif