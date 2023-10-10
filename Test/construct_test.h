#ifndef __CONSTRUCT_TEST_H__
#define __CONSTRUCT_TEST_H__

#include <iostream>
#include <stdlib.h>

#include "../MySTL/construct.h"

namespace construct_test{

// 记录一个bug，iterator_traits设置为class，默认私有
// 导致类型萃取失败，报错

class Myclass{
    private:
        int value;
    public:
        Myclass(int v) : value(v) {}
        ~Myclass() {
            value = -1;
            std::cout << "~Myclass : " << value << std::endl;
        }
        int get(){
            return value;
        }
};

void test() {
    std::cout << "------------construct_test-----------" << std::endl;
    std::cout << "--- test int ---" << std::endl;
    int* nums = (int *) malloc(10 * 4); // 申请10块内存

    for(int i = 0 ; i < 10 ; i++){
        // 在这十块int内存上面构造int对象
        mystl::construct(nums + i, i);
    }
    std::cout << "after construct ... " << std::endl;
    for(int i = 0 ; i < 10 ; i++){
        std::cout << nums[i] << " " ;
    }
    std::cout << std::endl;

    std::cout << "after destroy ... " << std::endl;
    mystl::destroy(nums, nums + 10);
    for(int i = 0 ; i < 10 ; i++){
        std::cout << nums[i] << " " ;
    }
    std::cout << std::endl;

    std::cout << "--- test class ---" << std::endl;
    Myclass* vec = (Myclass *) malloc(10 * sizeof(Myclass)); // 申请10块内存
    for(int i = 0 ; i < 10 ; i++){
        // 在这十块int内存上面构造int对象
        mystl::construct(vec + i, i);
    }
    std::cout << "after construct ... " << std::endl;
    for(int i = 0 ; i < 10 ; i++){
        std::cout << vec[i].get() << " " ;
    }
    std::cout << std::endl;

    std::cout << "after destroy ... " << std::endl;
    mystl::destroy(vec, vec + 10);
    for(int i = 0 ; i < 10 ; i++){
        std::cout << vec[i].get() << " " ;
    }
    std::cout << std::endl << std::endl;

    free(nums);
    free(vec);
}


} // namespace construct_test

#endif