#ifndef __MYSTL_ALLOCATOR_H__
#define __MYSTL_ALLOCATOR_H__

#include <stddef.h>

#include "construct.h"

// 这个头文件定义的是普通的封装operator::new和operator::delete的空间配置器
// 负责对象的内存分配和释放，采用类的静态函数使用，后续使用也是用类名::函数来使用

namespace mystl {
    
template <class Tp>
class allocator{
public:
    typedef Tp value_type;
    typedef Tp* pointer;
    typedef const Tp* const_pointer;
    typedef Tp& reference;
    typedef const Tp& const_reference;
    typedef ptrdiff_t difference_type;  // 64位signed
    typedef size_t size_type;           // 64位unsigned

    // 通过这个萃取出另一种型别的allocator
    template <class U>
    struct rebind {
        typedef allocator<U> other;
    };

    // 申请n个Tp类型的内存
    static pointer allocate(size_type n) {
        std::set_new_handler(0); // 不设置内存分配失败处理函数，失败抛出std::bad_alloc;
        pointer tmp = static_cast<pointer>(::operator new(n * sizeof(Tp)));   //operator new是按照字节分配的
        if(tmp == nullptr) {
            std::cerr << "out of memeory! allocator::allocate(size_type n)." << std::endl;
            exit(-1);
        }
        return tmp;
    }

    // 回收内存，不管是回收1个还是回收n个都是一样的，因为operator::new是连续申请的，不存在内存池
    static void deallocate(Tp* ptr) {
        if(ptr == nullptr) {
            return;
        }
        ::operator delete(ptr);
    }

    static void deallocate(Tp* ptr, size_type /* size */) {
        deallocate(ptr);
    }
};


}

#endif