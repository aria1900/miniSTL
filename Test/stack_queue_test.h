#ifndef __STACK_QUEUE_TEST_H__
#define __STACK_QUEUE_TEST_H__

#include <string>

#include "../MySTL/stack.h"
#include "../MySTL/queue.h"
#include "test.h"

namespace stack_queue_test{

void stack_print(mystl::stack<int> s)
{
  while (!s.empty())
  {
    std::cout << s.top() << " " ;
    s.pop();
  }
  std::cout << std::endl;
}

template <class Que>
void queue_print(Que q)
{
  while (!q.empty())
  {
    std::cout << q.front() << " " ;
    q.pop();
  }
  std::cout << std::endl;
}

template <class Que>
void priority_queue_print(Que q)
{
  while (!q.empty())
  {
    std::cout << q.top() << " " ;
    q.pop();
  }
  std::cout << std::endl;
}


// 负责输出stack 从栈顶
#define STACK_COUT(s) do {                          \
    std::string stack_name = #s;                    \
    std::cout << stack_name << " : ";               \
    stack_print(s);                                 \
} while(0)                                          

// 负责将操作后的stack输出
#define STACK_FUN_AFTER(st,fun) do {                \
    std::string fun_name = #fun;                    \
    std::cout << "after " << fun_name << " : ";     \
    fun;                                            \
    STACK_COUT(st);                                 \
} while(0)                                               


// queue 从队头输出
#define QUEUE_COUT(q) do {                          \
    std::string queue_name = #q;                    \
    std::cout << queue_name << " : ";               \
    queue_print(q);                                 \
} while(0)                                          

// 负责将操作后的stack输出
#define QUEUE_FUN_AFTER(que,fun) do {               \
    std::string fun_name = #fun;                    \
    std::cout << "after " << fun_name << " : ";     \
    fun;                                            \
    QUEUE_COUT(que);                                \
} while(0)                                          

// pri_queue 从队头输出
#define PRI_QUEUE_COUT(q) do {                      \
    std::string queue_name = #q;                    \
    std::cout << queue_name << " : ";               \
    priority_queue_print(q);                        \
} while(0)                                          

// 负责将操作后的stack输出
#define PRI_QUEUE_FUN_AFTER(que,fun) do {           \
    std::string fun_name = #fun;                    \
    std::cout << "after " << fun_name << " : ";     \
    fun;                                            \
    PRI_QUEUE_COUT(que);                            \
} while(0)                                          


void stack_test() {
    std::cout << "--------------------------stack test-----------------------" << std::endl;
    int a[] = { 1,2,3,4,5 };
    mystl::stack<int> s1;

    STACK_FUN_AFTER(s1, s1.push(1));
    STACK_FUN_AFTER(s1, s1.push(2));
    STACK_FUN_AFTER(s1, s1.push(3));
    STACK_FUN_AFTER(s1, s1.pop());
    std::cout << std::boolalpha;
    FUN_VALUE(s1.empty());
    std::cout << std::noboolalpha;
    FUN_VALUE(s1.size());
    FUN_VALUE(s1.top());
    while (!s1.empty())
    {
        STACK_FUN_AFTER(s1, s1.pop());
    }
    std::cout << std::endl;
}

void queue_test() {
    std::cout << "--------------------------queue test-----------------------" << std::endl;
    int a[] = { 1,2,3,4,5 };
    mystl::queue<int> q1;

    QUEUE_FUN_AFTER(q1, q1.push(1));
    QUEUE_FUN_AFTER(q1, q1.push(2));
    QUEUE_FUN_AFTER(q1, q1.push(3));
    QUEUE_FUN_AFTER(q1, q1.pop());
    std::cout << std::boolalpha;
    FUN_VALUE(q1.empty());
    std::cout << std::noboolalpha;
    FUN_VALUE(q1.size());
    FUN_VALUE(q1.front());
    while (!q1.empty())
    {
        QUEUE_FUN_AFTER(q1, q1.pop());
    }
}

void priority_queue_test() {
    std::cout << "--------------------------priority_queue test-----------------------" << std::endl;
    int a[] = { 1,2,3,4,5 };
    mystl::priority_queue<int> q1;

    PRI_QUEUE_FUN_AFTER(q1, q1.push(10));
    PRI_QUEUE_FUN_AFTER(q1, q1.push(-1));
    PRI_QUEUE_FUN_AFTER(q1, q1.push(48));
    PRI_QUEUE_FUN_AFTER(q1, q1.pop());
    std::cout << std::boolalpha;
    FUN_VALUE(q1.empty());
    std::cout << std::noboolalpha;
    FUN_VALUE(q1.size());
    FUN_VALUE(q1.top());
    while (!q1.empty())
    {
        PRI_QUEUE_FUN_AFTER(q1, q1.pop());
    }
    mystl::priority_queue<int> q2, q3;
    std::cout << (q2 == q3) << std::endl;
 
    std::cout << "sort_heap test" << std::endl;
    mystl::vector<int> vec = {23,134,5,-1,0,45 };
    mystl::make_heap(vec.begin(), vec.end());
    mystl::sort_heap(vec.begin(), vec.end());   // 得先建堆，才能堆排    
    COUT(vec);
}


}

#endif