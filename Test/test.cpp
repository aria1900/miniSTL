#include <iostream>

#include "construct_test.h"
#include "allocator_test.h"
#include "uninitialized_test.h"
#include "algobase_test.h"
#include "set_algo_test.h"
#include "algo_test.h"

#include "vector_test.h"
#include "list_test.h"
#include "deque_test.h"
#include "stack_queue_test.h"
#include "rb_tree_test.h"
#include "set_test.h"
#include "map_test.h"
#include "hashtable_test.h"
#include "unordered_set_test.h"
#include "unordered_map_test.h"

/*
*   本次测试在 Ubuntu 22.04     2核处理器       内存4GB
*   各类容器之间的表现 : +表示比std快  -表示比std慢  =表示两者相当
*   --------------------------------------------------------vector-----------------------------------------------------------
*   function        data_number            std::vector             mystl::vector            performence : (std - mystl) / std
*   push_back       1e                     1051ms                  788ms                    + faster: 25%
*   insert(front)   30w                    3479ms                  3494ms                   = almost

*   ----------------------------------------------------------list-----------------------------------------------------------
*   function        data_number            std::list                mystl::list             performence : (std - mystl) / std
*   insert(front)   1000w                  581ms                    420ms                   + faster: 28%
*   sort            1000w                  9329ms                   10231ms                 - slower: 10%

*   ----------------------------------------------------------deque----------------------------------------------------------
*   function        data_number            std::deque               mystl::deque            performence : (std - mystl) / std
*   push_back       1e                     893ms                    571ms                   + faster: 36%
*   push_front      1e                     740ms                    451ms                   + faster: 39%

*   ----------------------------------------------------------set------------------------------------------------------------
*   function        data_number            std::set                 mystl::set              performence : (std - mystl) / std
*   insert          1000w                  4075ms                   3091ms                  + faster: 24%

*   ---------------------------------------------------------map-------------------------------------------------------------
*   function        data_number            std::set                 mystl::set              performence : (std - mystl) / std
*   insert          1000w                  3874ms                   2955ms                  + faster: 24%

*   -----------------------------------------------------unordered_set-------------------------------------------------------
*   function        data_number            std::unordered_set       mystl::unordered_set    performence : (std - mystl) / std
*   insert          1000w                  990ms                    678ms                   + faster: 31%

*   -----------------------------------------------------unordered_map-------------------------------------------------------
*   function        data_number            std::unordered_map       mystl::unordered_map    performence : (std - mystl) / std
*   insert          1000w                  1215ms                   713ms                   + faster: 41%

*   ----------------------------------------------------------sort-----------------------------------------------------------
*   function        data_number            std::sort                mystl::sort             performence : (std - mystl) / std
*   sort            1e                     17691ms                  14231ms                 + faster: 20%


*/


int main() {
    /* construct_test::test();
    allocator_test::test();
    uninitialized_test::test();
    algobase_test::test();
    set_algo_test::test(); */
    algo_test::test();

    /* vector_test::test();
    list_test::test();
    deque_test::test(); */

    /* stack_queue_test::stack_test();
    stack_queue_test::queue_test();
    stack_queue_test::priority_queue_test(); */

    /* rb_tree_test::test();
    set_test::test();
    map_test::test(); 

    hashtable_test::test();
    unordered_set_test::test(); 
    unordered_map_test::test(); */

    

    return 0;
}