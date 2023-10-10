#ifndef __RB_TREE_TEST_H__
#define __RB_TREE_TEST_H__


#include "../MySTL/rb_tree.h"
#include "test.h"

namespace rb_tree_test {

template <class T1, class T2>
struct Comp {
    bool operator()(const mystl::pair<T1, T2>& x, const mystl::pair<T1, T2>& y) {
        return x.first < y.first;
    }
};

void test() {
    std::cout << "--------------------------rb_tree test-----------------------" << std::endl;
    mystl::rb_tree<int, int/* , Comp */> t1;    // key value
    t1.insert_unique(4);
    t1.insert_unique(5);
    t1.insert_unique(3);
    t1.insert_unique(2);
    t1.insert_unique(1);
    COUT(t1);
    mystl::rb_tree<int, int/* , Comp */> t2;
    t2 = t1;
    COUT(t2);
    t1.insert_equal(1);
    t1.insert_equal(4);
    t1.insert_equal(6);
    t2 = t1;
    /* for(int i = 20 ; i < 40 ; i++) {
        t1.insert_unique(i);
    } */
    t2 = t1;
    COUT(t2);

    mystl::rb_tree<int, int> t3;
    t3.insert_unique(t3.begin(), 3);
    t3.insert_equal(t3.begin(), 5);
    t3.insert_equal(t3.end(), -1);
    t3.insert_equal(t3.end(), 3);
    COUT(t3);
    FUN_VALUE(t3.size());
    FUN_VALUE(t3.count(3));
    auto it1 = t3.lower_bound(4);
    auto it2 = t3.upper_bound(4);
    std::cout << *it1 << " " << *it2 << std::endl;

    FUN_AFTER(t2, t2.erase(t2.begin()));
    FUN_AFTER(t2, t2.erase(4));
    //std::cout << t2.erase(4) << std::endl;
    FUN_AFTER(t2, t2.erase(t2.begin(), --t2.end()));
    std::cout << std::endl;
}


}


#endif