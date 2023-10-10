#ifndef __ALLOCATOR_H__
#define __ALLOCATOR_H__

#include <iostream>
#include <vector>
#include <deque>
#include <list>

#include "../MySTL/construct.h"
#include "../MySTL/allocator.h"

// 测试allocator是否能够正常的分配内存

namespace allocator_test {

class Myclass{
    private:
        int value;
    public:
        Myclass(int v = 12) : value(v) {}
        ~Myclass() {
            value = -1;
            std::cout << "~Myclass : " << value << std::endl;
        }
        int get(){
            return value;
        }
};


void test() {
    std::cout << "------------allocator_test-----------" << std::endl;
    typedef mystl::allocator<int> int_allocator;
    std::cout<< "allocate 10 * int memory " << std::endl;
    int* nums = int_allocator::allocate(10);
    std::cout<< "allocate success! " << std::endl;
    std::cout<< "construct object. " << std::endl;
    for(int i = 0 ; i < 10 ; i++) {
        mystl::construct(nums + i, i);
    }
    for(int i = 0 ; i < 10 ; i++) {
        std::cout << nums[i] << " ";
    }
    int_allocator::deallocate(nums);
    std::cout << std::endl;

    std::cout<< "allocate 10 * myclass memory " << std::endl;
    typedef mystl::allocator<Myclass> myclass_allocator;
    Myclass* vec = myclass_allocator::allocate(10);
    std::cout<< "allocate success! " << std::endl;
    std::cout<< "construct object. " << std::endl;
    for(int i = 0 ; i < 10 ; i++) {
        Myclass tmp(i);
        mystl::construct(vec + i, tmp);
    }
    for(int i = 0 ; i < 10 ; i++) {
        std::cout << vec[i].get() << " ";
    }
    std::cout << std::endl;
    mystl::destroy(vec, vec + 10);
    std::cout<< "deallocate." << std::endl;
    myclass_allocator::deallocate(vec);

    std::cout<< "use std::vector. " << std::endl;
    std::vector<int, int_allocator> numbers(10, 6);
    for(int num: numbers){
        std::cout << num << " ";
    }
    std::cout << std::endl << std::endl;
}

}

#endif