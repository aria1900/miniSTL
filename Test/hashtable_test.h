#ifndef __HASHTABLE_TEST_H__
#define __HASHTABLE_TEST_H__

#include "../MySTL/hashtable.h"
#include "test.h"

namespace hashtable_test {

void test() {
    std::cout << "--------------------------hashtable test-----------------------" << std::endl;
    mystl::hashtable<int,int> ht;
    
    int num[] = {1,2,3,4,5};
    ht.insert_unique(num, num + 5);
    ht.insert_equal(6);
    ht.insert_unique(7);
    std::cout<< *ht.begin() << std::endl;
    std::cout<< *(++ht.begin()) << std::endl;
    std::cout << ht.size() << std::endl;
    for(int i = 1000 ; i < 1500 ; i++) {
        ht.insert_unique(i * 2);
    } 
    std::cout << ht.bucket_count() << std::endl;
    /* for(int i = 0 ; i < ht.bucket_count() ; i++) {
        std::cout << "bucket " << i << " size : " <<
        ht.elems_in_bucket(i) << std::endl;
    }
    COUT(ht); */

    auto it = ht.begin();
    for(int n = 50 ; n > 0 ; --n) ++it;

    FUN_VALUE(ht.size());
    FUN_AFTER(ht, ht.erase(ht.begin(), it));
    FUN_VALUE(ht.size());
    FUN_AFTER(ht, ht.clear());
    
    FUN_AFTER(ht, ht.insert_unique(num, num + 5));
    FUN_AFTER(ht, ht.erase(3));
    FUN_AFTER(ht, ht.erase(ht.begin()));
    std::cout << std::endl;
}

}

#endif