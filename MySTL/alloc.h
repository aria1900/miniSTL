#ifndef __MYSTL_ALLOC_H__
#define __MYSTL_ALLOC_H__

#include <stddef.h>
#include <stdlib.h>
#include <new>

namespace mystl {

// 这是第一级配置器，底层采用malloc
template <int inst>
class malloc_alloc_template {
   private:
    static void* oom_malloc(size_t);  // malloc分配失败之后的oom机制
    static void* oom_realloc(void*, size_t);

    static void (
        *malloc_alloc_oom_handler)();  // oom处理函数，默认值为nullptr，一般有特定规格

   public:
    static void* allocate(size_t n) {
        void* result = ::malloc(n);
        if (nullptr == result) {
            result = oom_malloc(n);
        }
        return result;
    }

    // 这里不需要n个字节的指示
    static void deallocate(void* p, size_t /* n */) { free(p); }

    static void* reallocte(void* p, size_t old_sz, size_t new_sz) {
        void* result = ::realloc(p, new_sz);
        if (nullptr == result) {
            result = oom_realloc(p, new_sz);
        }
        return result;
    }

    // 传入一个 void()函数指针 返回一个void()函数指针
    static void (*set_malloc_handler(void (*f)()))() {
        void (*old)() = malloc_alloc_oom_handler;
        malloc_alloc_oom_handler = f;
        return old;
    }
};

// 初始化为空指针
template <int inst>
void (*malloc_alloc_template<inst>::malloc_alloc_oom_handler)() = nullptr;

template <int inst>
void* malloc_alloc_template<inst>::oom_malloc(size_t n) {
    void (*my_allocate_handle)();
    void* result;

    while (true) {
        my_allocate_handle = malloc_alloc_oom_handler;  // 获取oom处理函数
        if (nullptr == my_allocate_handle) {
            // 抛出异常
            throw std::bad_alloc();
        }
        my_allocate_handle();  // 处理oom情况
        result = ::malloc(n);
        if (result) {
            return result;
        }
    }
}

template <int inst>
void* malloc_alloc_template<inst>::oom_realloc(void* p, size_t n) {
    void (*my_allocate_handle)();
    void result;

    while (true) {
        my_allocate_handle = malloc_alloc_oom_handler;
        if (nullptr == my_allocate_handle) {
            throw std::bad_alloc();
        }
        my_allocate_handle();
        result = ::realloc(p, n);
        if (result) {
            return result;
        }
    }
}

typedef malloc_alloc_template<0> malloc_alloc;

// 以下为第二级配置器的实现
const int ALIGN = 8;
const int MAX_BYTES = 128;
const int NFREELISTS = 16;  // 16个free_lists

template <bool threads, int inst>
class default_alloc_template {
   private:
    // 把字节数上调至8的倍数
    static size_t round_up(size_t bytes) {
        return (bytes + (size_t)ALIGN - 1) & ~(size_t(ALIGN) - 1);
    }

    // 内存块结点
    union Obj {
        union Obj* volatile* free_list_link;
        char client_data[1]; /* The client sees this.        */
    };

    static Obj* volatile* free_lists_[];  // 自由链表，存放内存块指针

    // 能够计算内存块大小所在的freelists下标
    static size_t freelist_index(size_t bytes) {
        return (bytes + size_t(ALIGN) - 1) / size_t(ALIGN) - 1;
    }

    // 重新填充freelists链表，并且返回第一个内存块
    static void* refill(size_t n);

    // 向内存池索要内存，并且返回输出参数nobjs实际获得的块数
    static char* chunk_alloc(size_t size, int& nobjs);

    // 内存池的参数
    static char* start_free_;
    static char* end_free_;
    static size_t heap_size;  // 内存池的大小 B为单位

   public:
    // 对外的接口
    static void* allocate(size_t n) {
        void* ret = nullptr;
        if (n > size_t(MAX_BYTES)) {
            ret = malloc_alloc::allocate(n);
        } else {
            Obj* volatile* my_free_list = free_lists_ + freelist_index(n);
            Obj* result = *my_free_list;  // 直接取第一个内存块
            if (nullptr == result) {
                ret = refill((round_up(n)));  // 问内存池索要内存
            } else {
                // 链上摘取成功
                *my_free_list = result->free_list_link;
                ret = result;
            }
        }
        return ret;
    }

    static void deallocate(void* p, size_t n) {
        if (n > size_t(MAX_BYTES)) {
            malloc_alloc::deallocate(p, n);  // 大于128B，交给第一级配置器回收
        } else {
            Obj* volatile* my_free_list =
                free_lists_ + freelist_index(n);  // 获得下标指针
            Obj* q = (Obj*)p;
            q->free_list_link = *my_free_list;
            *my_free_list = q;
        }
    }

    static void* reallocate(void* p, size_t old_sz, size_t new_sz) {
        void* result;
        size_t copy_sz;
        if (old_sz > (size_t)MAX_BYTES && new_sz > (size_t)MAX_BYTES) {
            return malloc_alloc::reallocte(p, new_sz);
        }
        // 旧内存和新内存相同
        if (round_up(old_sz) == round_up(new_sz)) {
            return p;
        }
        result = allocate(new_sz);
        copy_sz = new_sz > old_sz ? old_sz : new_sz;  // 选小的
        memcpy(result, p, copy_sz);
        deallocate(p, old_sz);  // 释放内存
        return result;
    }
};

template <bool threads, int inst>
char* default_alloc_template<threads, inst>::chunk_alloc(size_t size,
                                                         int& nobjs) {
    char* result;
    size_t total_bytes = size * nobjs;            // 需要的内存
    size_t bytes_left = end_free_ - start_free_;  // 内存池还剩下的内存

    if (bytes_left >= total_bytes) {
        // 内存池的内存足够
        result = start_free_;
        start_free_ += total_bytes;
        return result;
    } else if (bytes_left >= size) {
        // 有着1~19内存块的容量
        nobjs = (int)(bytes_left / size);
        total_bytes = size * nobjs;
        result = start_free_;
        start_free_ += total_bytes;
        return result;
    } else {
        // 内存池上一个内存块的容量也没有了

        // 这里至少准备向malloc申请40个内存块
        size_t bytes_to_get = 2 * total_bytes + round_up(heap_size >> 4);
        if (bytes_left > 0) {
            // 内存池还有剩余，那么挂载到free_lists上去
            Obj* volatile* my_free_list =
                free_lists_ + freelist_index(bytes_left);
            ((Obj*)start_free_)->free_list_link = *my_free_list;  // 头插
            *my_free_list = (Obj*)start_free_;
        }

        start_free_ = (char*)malloc(bytes_to_get);
        if (0 == start_free_) {
            // 没有申请到
            // 先尝试将大于size的free_lists当做内存池
            size_t i;
            Obj* volatile* my_free_lists;
            Obj* p;
            for (i = size; i <= (size_t)MAX_BYTES; i += (size_t)ALIGN) {
                my_free_lists = free_lists_ + freelist_index(i);
                p = *my_free_lists;
                if (0 != p) {
                    // 说明链上有
                    *my_free_lists = p->free_list_link;
                    start_free_ = (char*)p;
                    end_free_ = start_free_ + i;
                    return chunk_alloc(size, nobjs);  // 会走到分支2
                }
            }

            // 在考虑第一级配置器的oom机制，一般会std::bad_alloc()
            end_free_ = nullptr;
            start_free_ = (char*)malloc_alloc::allocate(bytes_to_get);
        }

        heap_size += bytes_to_get;  // 总内存，不但包括lists的还包含内存池的
        end_free_ = start_free_ + bytes_to_get;
        return (chunk_alloc(size, nobjs));  // 会走到分支1
    }
}

// 主要的作用是，链表为空时，向内存池索要内存，第一块返回给客端，其他的分割，然后链到链表上
template <bool threads, int inst>
void* default_alloc_template<threads, inst>::refill(size_t n) {
    int nobjs = 20;
    char* chunk = chunk_alloc(n, nobjs);  // 输出参数nobjs，实际分配的内存块数
    Obj* volatile* my_free_list;
    Obj* result;
    Obj* current_obj;
    Obj* next_obj;
    int i;

    if (1 == nobjs) {
        return chunk;
    }

    my_free_list = free_lists_ + freelist_index(n);
    result = (Obj*)chunk;
    *my_free_list = next_obj =
        (Obj*)(chunk + n);  // char类型的，指向free_list该指向的
    for (i = 1;; i++) {
        current_obj = next_obj;
        next_obj = (Obj*)((char*)next_obj + n);  // 指向下一个内存块
        if (nobjs - 1 = i) {
            // 最后一个区块
            current_obj->free_list_link = nullptr;
            break;
        } else {
            current_obj->free_list_link = next_obj;
        }
    }
    return result;
}

template <bool threads, int inst>
char* default_alloc_template<threads, inst>::start_free_ = nullptr;

template <bool threads, int inst>
char* default_alloc_template<threads, inst>::end_free_ = nullptr;

template <bool threads, int inst>
size_t default_alloc_template<threads, inst>::heap_size = 0;

template <bool threads, int inst>
typename default_alloc_template<threads, inst>::Obj* volatile*
    default_alloc_template<threads, inst>::free_lists_[] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

}  // namespace mystl

#endif