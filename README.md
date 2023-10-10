MYSTL
简介
​ 本项目仿照GNU 2.9版本以及Alinshans版本的STL完成了一个属于自己的MYSTL。实现了STL六大组件的大部分内容，包括容器、配置器、仿函数和算法等等。

参考实现: https://github.com/Alinshans/MyTinySTL

​ https://github.com/karottc/sgi-stl

运行
本项目有自己构建的测试，比较随意，但是基本上满足要求。

g++ on linux

构建
// clone到本地以后
& cmake
& cd build & make
运行
& cd bin
& ../stltest
代码结构组成
1. Allocator --- 配置器
1.1 allocator
​ 内存的分配与回收。

allocate
deallocate
1.2 constructor（全局函数）
​ 对象的构建与析构。

construct
destroy
2. Iterator --- 迭代器
2.1 iterator
​ 每种容器定义了自己的iterator。

3. Container --- 容器
3.1 vector
动态数组，支持动态扩容，线性连续空间。

3.2 list
双向链表，不连续空间。

3.3 deque
双端队列，支持高效的前插后插，对外体现连续空间。

3.4 set / multiset
集合，有序。前者不允许键值重复，后者允许键值重复。查找插入为O(logn)

3.5 map / multimap
映射，有序。前者不允许键值重复，后者允许键值重复。查找插入为O(logn)

3.6 unordered_set / unordered_multiset
无序集合。元素本身无序存放与容器中。前者不允许键值重复，后者允许键值重复。查找插入为O(1)。

3.7 unordered_map / unordered_multiset
无序映射。元素本身无序存放与容器中。前者不允许键值重复，后者允许键值重复。查找插入为O(1)。

4. Functor --- 仿函数
定义了部分仿函数。可配接。

5. Algorithm --- 算法
5.1 基本算法
5.2 数值算法
5.3 set算法
5.4 heap算法
5.5 其他算法
6. Adapter --- 配接器
6.1 container adapters
​ stack
​ queue
​ priority_queue
6.2 function adapters
bind1st
bind2st
6.3 iterator adapters
reverse_iterator
insert_iterator
istream_iterator
ostream_iterator
