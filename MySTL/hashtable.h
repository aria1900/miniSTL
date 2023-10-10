#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include <initializer_list>

#include "algo.h"
#include "functional.h"
#include "memory.h"
#include "vector.h"
#include "util.h"
#include "exceptdef.h"


namespace mystl {

template <class T>
struct hashtable_Node {
    hashtable_Node* next;       // 下一个结点指针
    T               value;      // 值域

    hashtable_Node() = default;
    hashtable_Node(const T& n) : next(nullptr), value(n) {}
    hashtable_Node(const hashtable_Node& node) : next(node.next), value(node.value) {}
    hashtable_Node(hashtable_Node&& node) : next(node.next), value(mystl::move(node.value)) {
        node.next = nullptr;
    }
};

template <class Key, class Value, class HashFun, class KeyOfValue, class EqualKey>
class hashtable;

template <class Key, class Value, class HashFun, class KeyOfValue, class EqualKey>
struct hashtable_iterator;

template <class Key, class Value, class HashFun, class KeyOfValue, class EqualKey>
struct hashtable_const_iterator;

template <class Key, class Value, class HashFun, class KeyOfValue, class EqualKey>
struct hashtable_iterator {
    typedef hashtable<Key, Value, HashFun, KeyOfValue, EqualKey>                    Hashtable;
    typedef hashtable_iterator<Key, Value, HashFun, KeyOfValue, EqualKey>           iterator;
    typedef hashtable_const_iterator<Key, Value, HashFun, KeyOfValue, EqualKey>     const_iterator;
    typedef hashtable_Node<Value>                                                   Node;
    typedef hashtable_Node<Value>*                                                  node_ptr;
    typedef Hashtable*                                                              table_ptr;      // 指向哈希表buckets的指针，也就是vector*

    typedef forward_iterator_tag    iterator_category;
    typedef Value                   value_type;
    typedef ptrdiff_t               difference_type;
    typedef size_t                  size_type;
    typedef Value*                  pointer;
    typedef Value&                  reference;

    node_ptr    cur;    // 指向node本身
    table_ptr   ht;     // 指向哈希表，方便移动

    hashtable_iterator() = default;
    hashtable_iterator(node_ptr n, table_ptr tab) : cur(n), ht(tab) {}
    hashtable_iterator(const iterator& rhs) : cur(rhs.cur), ht(rhs.ht) {}
    iterator& operator=(const iterator& rhs) {
        cur = rhs.cur;
        ht = rhs.ht;
        return *this;
    }
    ~hashtable_iterator() = default;

    // 运算符重载
    reference   operator*()     const { return cur->value; }
    pointer     operator->()    const { return &(operator*()); }

    iterator& operator++();
    iterator  operator++(int);

    bool operator==(const iterator& it) const { return  cur == it.cur; }
    bool operator!=(const iterator& it) const { return  cur != it.cur; }
};

// const_iterator
template <class Key, class Value, class HashFun, class KeyOfValue, class EqualKey>
struct hashtable_const_iterator {
    typedef hashtable<Key, Value, HashFun, KeyOfValue, EqualKey>                    Hashtable;
    typedef hashtable_iterator<Key, Value, HashFun, KeyOfValue, EqualKey>           iterator;
    typedef hashtable_const_iterator<Key, Value, HashFun, KeyOfValue, EqualKey>     const_iterator;
    typedef hashtable_Node<Value>                                                   Node;
    typedef hashtable_Node<Value>*                                                  node_ptr;
    typedef Hashtable*                                                              table_ptr;      // 指向哈希表buckets的指针，也就是vector*

    typedef forward_iterator_tag    iterator_category;
    typedef Value                   value_type;
    typedef ptrdiff_t               difference_type;
    typedef size_t                  size_type;
    typedef Value*                  pointer;
    typedef Value&                  reference;

    node_ptr    cur;    // 指向node本身
    table_ptr   ht;     // 指向哈希表，方便移动

    hashtable_const_iterator() = default;
    hashtable_const_iterator(const node_ptr n, const table_ptr tab) : cur(n), ht(tab) {}
    hashtable_const_iterator(const const_iterator& rhs) : cur(rhs.cur), ht(rhs.ht) {}
    const_iterator& operator=(const const_iterator& rhs) {
        cur = rhs.cur;
        ht = rhs.ht;
        return *this;
    }
    ~hashtable_const_iterator() = default;

    // 运算符重载
    reference   operator*()     const { return cur->value; }
    pointer     operator->()    const { return &(operator*()); }

    const_iterator& operator++();
    const_iterator  operator++(int);

    bool operator==(const const_iterator& it) const { return  cur == it.cur; }
    bool operator!=(const const_iterator& it) const { return  cur != it.cur; }
};


// 质数表
const int num_primes = 28;

static const unsigned long ht_prime_list[num_primes] =
{
  53ul,         97ul,         193ul,       389ul,       769ul,
  1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
  49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
  1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
  50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul, 
  1610612741ul, 3221225473ul, 4294967291ul
};

// 返回n的下一个质数
unsigned long next_prime(unsigned long n) {
    const unsigned long* first = ht_prime_list;
    const unsigned long* last = ht_prime_list + num_primes;
    const unsigned long* pos = mystl::lower_bound(first, last, n);
    return pos == last ? *(last - 1) : *pos;     // pos == last 就说明n比最大的还要大了
}
 

// 哈希表，第一个参数是key，第二个参数是node存放的真正的值，第三个参数是对key进行散列函数，第四个参数是对value提取key，第五个参数是判断key是否相等
template <class Key, class Value, class HashFun = mystl::hash<Key>, 
          class KeyOfValue = mystl::identity<Value>, class EqualKey = mystl::equal_to<Key>>
class hashtable {
public:
    typedef     Key                         key_type;
    typedef     Value                       value_type;
    typedef     HashFun                     hasher;         // 用来对key进行hash的
    typedef     EqualKey                    key_equal;
    typedef     hashtable_Node<Value>       Node;
    typedef     hashtable_Node<Value>*      node_ptr;

    typedef     size_t              size_type;
    typedef     ptrdiff_t           difference_type;
    typedef     value_type*         pointer;
    typedef     const value_type*   const_pointer;
    typedef     value_type&         reference;
    typedef     const value_type&   const_reference;

    typedef     mystl::allocator<Value>     allocator_type;
    typedef     mystl::allocator<Value>     data_allocator;
    typedef     mystl::allocator<Node>      node_allocator;

    typedef hashtable_iterator<Key, Value, HashFun, KeyOfValue, EqualKey>           iterator;
    typedef hashtable_const_iterator<Key, Value, HashFun, KeyOfValue, EqualKey>     const_iterator;     

    hasher      hash_funct()    const { return  hash_; }
    key_equal   key_eq()        const { return  equals_; }
    KeyOfValue  get_key()       const { return  get_key_; }

    allocator_type  get_allocator() const { return allocator_type(); }

    // 一定要声明有元，否则访问不了
    friend struct hashtable_iterator<Key, Value, HashFun, KeyOfValue, EqualKey>;
    friend struct hashtable_const_iterator<Key, Value, HashFun, KeyOfValue, EqualKey>;

private:
    // 用六个参数来表现hashtable
    hasher                      hash_;      // 对key进行hash的仿函数
    key_equal                   equals_;    // 判断key是否相等的仿函数
    KeyOfValue                  get_key_;   // 从value中提取出来key的仿函数
    mystl::vector<node_ptr>     buckets_;   // 哈希表，存放node指针，采用拉链法
    size_type                   num_elems_; // 有多少个哈希node

public:
    // 构造、拷贝、移动和析构函数

    hashtable() {
        init_buckets(50);    // 将bucket设置为next n的大小，同时num_elems设为0
    }

    // 初始化必须得有个n表示n个元素
    hashtable(size_type n, const hasher& hf, const key_equal& eql, const KeyOfValue& kov) 
        : hash_(hf), equals_(eql), get_key_(kov) {
        init_buckets(n);    // 将bucket设置为next n的大小，同时num_elems设为0
    }

    hashtable(const hashtable& ht) : hash_(ht.hash_), equals_(ht.equals_), get_key_(ht.get_key_), num_elems_(0) {
        copy_from(ht);  // 包括buckets的内存分配, 以及num的设置
    }

    // 将另一个资源掠夺，并置空另一个ht
    hashtable(hashtable&& rhs) : hash_(rhs.hash_), equals_(rhs.equals_), get_key_(rhs.get_key_),
                                buckets_(mystl::move(rhs.buckets_)) , num_elems_(rhs.num_elems_) {
        rhs.num_elems_ = 0; // 由于rhs.buckets已经经由move copy掠夺了，所以不用置空另一个的vector
    }

    hashtable& operator=(const hashtable& rhs) {
        if(&rhs != this) {
            clear();        // 清空自身资源
            hash_ = rhs.hash_;
            equals_ = rhs.equals_;
            get_key_ = rhs.get_key_;
            copy_from(rhs);
        }
        return *this;
    }

    // move operator= , 先释放自己的资源，再掠夺别人的资源，最后再将别人的资源指针置空
    hashtable& operator=(hashtable&& rhs) {
        clear();        // 清空自身资源
        hash_ = rhs.hash_;
        equals_ = rhs.equals_;
        get_key_ = rhs.get_key_;
        buckets_ = mystl::move(rhs.buckets_);
        num_elems_ = rhs.num_elems_;

        rhs.num_elems_ = 0;
        return *this;
    }

    ~hashtable() { clear(); }

    // 迭代器相关操作，由于迭代器为前向迭代器，所以没有reverse_iterator
    iterator            begin() { return begin_aux(); }
    const_iterator      begin() const { return begin_aux(); }
    iterator            end()   { return iterator(nullptr, this); } // nullptr才是end的Node*
    const_iterator      end()   const { return const_iterator(nullptr, this); }

    const_iterator cbegin() const { return begin(); }
    const_iterator cend()   const { return end(); }

    bool empty() const { return num_elems_ == 0; } 
    size_type size() const { return num_elems_; }
    size_type max_size() const { return static_cast<size_type>(-1); }

    void swap(hashtable& rhs) {
        if(this != &rhs) {
            buckets_.swap(rhs.buckets_);
            mystl::swap(hash_, rhs.hash_);
            mystl::swap(equals_, rhs.equals_);
            mystl::swap(get_key_, rhs.get_key_);
            mystl::swap(num_elems_, rhs.num_elems_);
        }
    }

public:
    // 返回buckets的大小
    size_type bucket_count() const { return buckets_.size(); }

    size_type max_bucket_count() const { return ht_prime_list[num_primes - 1]; }

    // 返回当前散列的个数
    size_type elems_in_bucket(size_type bucket_idx) const {
        size_type result = 0;
        for(node_ptr cur = buckets_[bucket_idx] ; cur ; cur = cur->next) {
            ++result;
        }
        return result;
    }

    // insert
    mystl::pair<iterator, bool> insert_unique(const value_type& value) {
        resize(num_elems_ + 1);
        return insert_unique_noresize(value);
    }
    iterator insert_equal(const value_type& value) {
        resize(num_elems_ + 1);
        return insert_equal_noresize(value);
    }

    template <class Iterator>
    void insert_unique(Iterator first, Iterator last) {
        size_type n = mystl::distance(first, last);
        resize(num_elems_ + n); // 这个只是扩容，并未对num_elems调整
        for(; n > 0 ; --n, ++first) {
            insert_unique_noresize(*first);
        }
    }

    template <class Iterator>
    void insert_equal(Iterator first, Iterator last) {
        size_type n = mystl::distance(first, last);
        resize(num_elems_ + n);
        for(; n > 0 ; --n, ++first) {
            insert_equal_noresize(*first);
        }
    }

    
    // erase
    size_type erase(const key_type& key);
    iterator erase(const iterator& it);
    void erase(iterator first, iterator last);


    // 传入的参数是新的结点个数，如果结点个数大于bucket的个数，则会重构哈希表，否则什么都不干
    // 所以在插入之类的，让结点数量增加的地方，可以用这个来试图重构
    void resize(size_type num_elems_hint); 

    // find
    iterator find(const key_type& key) {
        const size_type n = bkt_num_key(key);
        node_ptr first;
        // 找到相等的
        for(first = buckets_[n] ; first && !equals_(get_key_(first->value), key) ; first = first->next) { }

        return iterator(first, this);   // 有可能最后找不到，first 为nullptr，也就是end
    }

    size_type count(const key_type& key) const {
        const size_type n = bkt_num_key(key);
        size_type result = 0;
        for(node_ptr cur = buckets_[n] ; cur ; cur = cur->next) {
            if(equals_(get_key_(cur->value), key)) {
                ++result;
            }
        }
        return result;
    }

    pair<iterator, iterator>
    equal_range(const key_type& key);


    // 将所有node清空释放，vector不用管，这个会自动释放
    void clear() {
        erase(begin(), end());
    }  

private:
    // 辅助函数

    // 找到哈希表的第一个元素的辅助函数
    iterator begin_aux() {
        for(size_type n = 0 ; n < buckets_.size() ; ++n) {
            if(buckets_[n]) {
                // 找到了不为空的第一个元素
                return iterator(buckets_[n], this);     // node* hashtable* 构造函数
            }
        }
        return iterator(nullptr, this);     // end
    }

    const_iterator begin_aux() const {
        for(size_type n = 0 ; n < buckets_.size() ; ++n) {
            if(buckets_[n]) {
                return const_iterator(buckets_[n], this);     
            }
        }
        return const_iterator(nullptr, this); 
    }

    // 初始化n个元素的buckets，当然长度由prime决定
    void init_buckets(size_type n) {
        const size_type size = next_size(n);
        buckets_.reserve(size);        // 预留n个空间
        buckets_.insert(buckets_.end(), size, nullptr);     // 全部初始化为nullptr
        num_elems_ = 0;     // 结点数量设置为0
    }


    // 创建和释放结点，包括对象构造和析构
    node_ptr create_node(const value_type& value) {
        node_ptr node = node_allocator::allocate(1);
        node->next = nullptr;
        try {
            mystl::construct(&node->value, value);      // 在值域调用构造函数
            return node;
        }catch(...) {
            node_allocator::deallocate(node);
        }
    }

    void destroy_node(node_ptr n) {
        mystl::destroy(&n->value);  // 析构对象
        node_allocator::deallocate(n);
    }

    key_type get_key_from_value(const value_type& value) {
        return get_key_(value);
    }

    // 返回key所属的hashtable下标(vector)
    // hash是为了得到size_t的hash_code, 下标映射就是取模
    size_type bkt_num_key(const key_type& key, size_t size) const {
        return hash_(key) % size;  
    }

    size_type bkt_num_key(const key_type& key) const {
        return bkt_num_key(key, buckets_.size());
    }

    /* // 设置带value的映射
    size_type bkt_num_key(const value_type& value) const {
        return bkt_num_key(get_key_(value));
    } */

    // 靠近n的下一个质数
    size_type next_size(size_type n) const {
        return next_prime(n);
    }


    // 真正的插入函数,已经做过resize操作了
    pair<iterator, bool> insert_unique_noresize(const value_type& value);
    iterator insert_equal_noresize(const value_type& value);


    // 从另一个哈希表中复制到这里来
    void copy_from(const hashtable& ht);

    // 删除一个bucket的[first, last]
    void erase_bucket(const size_type& n, node_ptr first, node_ptr last);
};

// 重载迭代器++
// 这里是通过value的值，找到buckets的下标(通过散列)，然后顺着这个下标找，如果都找不到那就是nullptr
template <class Key, class Value, class HashFun, class KeyOfValue, class EqualKey>
typename hashtable_iterator<Key, Value, HashFun, KeyOfValue, EqualKey>::iterator& 
hashtable_iterator<Key, Value, HashFun, KeyOfValue, EqualKey>::operator++() {
    const node_ptr old = cur;
    cur = cur->next;
    if(!cur) {
        // cur移动到当前链表的末尾了
        size_type bucket_idx = ht->bkt_num_key(ht->get_key_(old->value));
        while(!cur && ++bucket_idx < ht->buckets_.size()) {
            cur = ht->buckets_[bucket_idx];
        }
    }
    return *this;
}

template <class Key, class Value, class HashFun, class KeyOfValue, class EqualKey>
typename hashtable_iterator<Key, Value, HashFun, KeyOfValue, EqualKey>::iterator 
hashtable_iterator<Key, Value, HashFun, KeyOfValue, EqualKey>::operator++(int) {
    iterator tmp = *this;
    ++*this;
    return tmp;
}

// const_iterator
template <class Key, class Value, class HashFun, class KeyOfValue, class EqualKey>
typename hashtable_const_iterator<Key, Value, HashFun, KeyOfValue, EqualKey>::const_iterator& 
hashtable_const_iterator<Key, Value, HashFun, KeyOfValue, EqualKey>::operator++() {
    const node_ptr old = cur;
    cur = cur->next;
    if(!cur) {
        // cur移动到当前链表的末尾了
        size_type bucket_idx = ht->bkt_num_key(ht->get_key_(old->value));
        while(!cur && ++bucket_idx < ht->buckets_.size()) {
            cur = ht->buckets_[bucket_idx];
        }
    }
    return *this;
}

template <class Key, class Value, class HashFun, class KeyOfValue, class EqualKey>
typename hashtable_const_iterator<Key, Value, HashFun, KeyOfValue, EqualKey>::const_iterator
hashtable_const_iterator<Key, Value, HashFun, KeyOfValue, EqualKey>::operator++(int) {
    const_iterator tmp = *this;
    ++*this;
    return tmp;
}

// hashtable
template <class Key, class Value, class HashFun, class KeyOfValue, class EqualKey>
void hashtable<Key, Value, HashFun, KeyOfValue, EqualKey>::resize(size_type num_elems_hint) {
    //std::cout << "resize begin" << std::endl; 
    const size_type old_n = buckets_.size();
    if(num_elems_hint > old_n) {
        // 需要重构了
        const size_type n = next_size(num_elems_hint);  // 下一个质数
        if(n > old_n) {
            mystl::vector<node_ptr> tmp(n, nullptr);
            // 将原来的node移动到新的hash表
            for(size_type bucket = 0 ; bucket < old_n ; ++ bucket) {
                node_ptr first = buckets_[bucket];
                while(first) {
                    size_type new_bucket = bkt_num_key(get_key_(first->value), n);    // 这里要用n，否则用旧的size哈希了
                    buckets_[bucket] = first->next;
                    first->next = tmp[new_bucket];
                    tmp[new_bucket] = first;
                    first = buckets_[bucket];
                }
            }
            buckets_.swap(tmp); // 这里完成旧hash表的内存释放
        }
        // else 否则不需要在扩容
    }
    //std::cout << "resize end" << std::endl; 
}

template <class Key, class Value, class HashFun, class KeyOfValue, class EqualKey>
pair<typename hashtable<Key, Value, HashFun, KeyOfValue, EqualKey>::iterator, bool> 
hashtable<Key, Value, HashFun, KeyOfValue, EqualKey>::insert_unique_noresize(const value_type& value) {
    const size_type n = bkt_num_key(get_key_(value));
    node_ptr first = buckets_[n];

    for(node_ptr cur = first ; cur ; cur = cur->next) {
        if(equals_(get_key_(cur->value), get_key_(value))) {
            // 说明有重复key
            return pair<iterator, bool>(iterator(cur, this), false);
        }
    }

    node_ptr tmp = create_node(value);
    tmp->next = first;
    buckets_[n] = tmp;
    ++num_elems_;
    return pair<iterator, bool>(iterator(tmp, this), true);
}

template <class Key, class Value, class HashFun, class KeyOfValue, class EqualKey>
typename hashtable<Key, Value, HashFun, KeyOfValue, EqualKey>::iterator 
hashtable<Key, Value, HashFun, KeyOfValue, EqualKey>::insert_equal_noresize(const value_type& value) {
    const size_type n = bkt_num_key(get_key_(value));
    node_ptr first = buckets_[n];

    for(node_ptr cur = first ; cur ; cur = cur->next) {
        if(equals_(get_key_(cur->value), get_key_(value))) {
            // 说明有重复key, 在第一个key后面插入，不能保证稳定性
            node_ptr tmp = create_node(value);
            tmp->next = cur->next;
            cur->next = tmp;
            ++num_elems_;
            return iterator(tmp, this);
        }
    }

    node_ptr tmp = create_node(value);
    tmp->next = first;
    buckets_[n] = tmp;
    ++num_elems_;
    return iterator(tmp, this);
}

template <class Key, class Value, class HashFun, class KeyOfValue, class EqualKey>
pair<typename hashtable<Key, Value, HashFun, KeyOfValue, EqualKey>::iterator, 
     typename hashtable<Key, Value, HashFun, KeyOfValue, EqualKey>::iterator>
hashtable<Key, Value, HashFun, KeyOfValue, EqualKey>::equal_range(const key_type& key) {
    typedef pair<iterator, iterator> Pair;
    const size_type n = bkt_num_key(key);

    for(node_ptr first = buckets_[n] ; first ; first = first->next) {
        if(equals_(get_key_(first->value), key)) {
            // 找到了第一个key
            for(node_ptr cur = first->next ; cur ; cur = cur->next) {
                if(!equals_(get_key_(cur->value), key)) {
                    // 找到了第一个不为key的，且不为空
                    return Pair(iterator(first, this), iterator(cur, this));
                }
            }
            // 该bucket没找到，非空的last，去下一个
            for(size_type m = n + 1 ; m < buckets_.size() ; ++m) {
                if(buckets_[m]) {
                    // 非空就是last
                    return Pair(iterator(first, this), iterator(buckets_[m], this));
                }
            }
            return Pair(iterator(first, this), iterator(end()));
        }
    }

    return Pair(end(), end());  // 没找到
}

template <class Key, class Value, class HashFun, class KeyOfValue, class EqualKey>
typename hashtable<Key, Value, HashFun, KeyOfValue, EqualKey>::size_type 
hashtable<Key, Value, HashFun, KeyOfValue, EqualKey>::erase(const key_type& key) {
    const size_type n = bkt_num_key(key);
    node_ptr first = buckets_[n];
    size_type erased_num = 0;

    // first不为空，说明可能在这里
    if(first) {
        node_ptr cur = first;
        node_ptr next = first->next;
        // 要删除的是next，最后再考虑第一个结点的问题
        while(next) {
            if(equals_(get_key_(next->value), key)) {
                // 删除next
                cur->next = next->next;
                destroy_node(next);
                next = cur->next;
                --num_elems_;
                ++erased_num;
            }else {
                // 指针移动
                cur = next;
                next = next->next;
            }
        }
        // 除了第一个结点，都被删除了，这下考虑第一个
        if(equals_(get_key_(first->value), key)) {
            buckets_[n] = first->next;
            destroy_node(first);
            --num_elems_;
            ++erased_num;
        }
    }

    return erased_num;
}

template <class Key, class Value, class HashFun, class KeyOfValue, class EqualKey>
typename hashtable<Key, Value, HashFun, KeyOfValue, EqualKey>::iterator  
hashtable<Key, Value, HashFun, KeyOfValue, EqualKey>::erase(const iterator& it) {
    node_ptr p = it.cur;
    if(p) {
        const size_type n = bkt_num_key(get_key_(p->value));
        node_ptr cur = buckets_[n];

        if(cur == p) {
            // 在第一结点
            buckets_[n] = cur->next;
            destroy_node(cur);
            --num_elems_;
            return iterator(buckets_[n], this);
        }else{
            // 不在第一个结点
            node_ptr next = cur->next;
            while(next) {
                if(next == p) {
                    cur->next = next->next;
                    destroy_node(p);
                    next = cur->next;
                    --num_elems_;
                    return iterator(next, this);
                }else {
                    // 移动
                    cur = next;
                    next = next->next;
                }
            }
        }
    }
    return end();   // 如果没有这个it的话
}

template <class Key, class Value, class HashFun, class KeyOfValue, class EqualKey>
void hashtable<Key, Value, HashFun, KeyOfValue, EqualKey>::erase(iterator first, iterator last) {
    size_type f_bucket = first.cur ? bkt_num_key(get_key_(first.cur->value)) : buckets_.size();     // 空说明是end
    size_type l_bucket = last.cur ? bkt_num_key(get_key_(last.cur->value)) : buckets_.size();

    if(first.cur == last.cur) 
        return;
    else if(f_bucket == l_bucket) {
        // 在一个bucekt中
        erase_bucket(f_bucket, first.cur, last.cur);        // 清除f_bucket中的[first, last) node
    }else {
        erase_bucket(f_bucket, first.cur, nullptr); // [first, end)
        for(size_type n = f_bucket + 1 ; n < l_bucket ; ++n) {
            erase_bucket(n, buckets_[n], nullptr);      // 全部删除
        }
        if(l_bucket != buckets_.size()) {
            // l_bucket == buckets.size() 说明正好last处于end
            erase_bucket(l_bucket, buckets_[l_bucket], last.cur);
        }
    }
}

template <class Key, class Value, class HashFun, class KeyOfValue, class EqualKey>
void hashtable<Key, Value, HashFun, KeyOfValue, EqualKey>::erase_bucket(const size_type& n, node_ptr first, node_ptr last) {
    if(first == last) return;       // 排除空链表
    node_ptr cur = buckets_[n];
    if(cur == first) {
        // 最后再删除first
        node_ptr next = cur->next;
        while(next != last) {
            cur->next = next->next;
            destroy_node(next);
            next = cur->next;
            --num_elems_;
        }
        buckets_[n] = first->next;
        destroy_node(first);
        --num_elems_;
    }else {
        // first在中间
        node_ptr next;
        for(next = cur->next ; next != first; cur = next, next = next->next); // next 移动到first位置，开始删除

        while(next != last) {
            cur->next = next->next;
            destroy_node(next);
            next = cur->next;
            --num_elems_;
        }
    }
}

// copy_from 深拷贝
template <class Key, class Value, class HashFun, class KeyOfValue, class EqualKey>
void hashtable<Key, Value, HashFun, KeyOfValue, EqualKey>::copy_from(const hashtable& ht) {
    buckets_.clear();       // 先清空自己，预留空间
    buckets_.reserve(ht.buckets_.size());
    buckets_.insert(buckets_.end(), ht.buckets_.size(), nullptr);

    // 开始一个node一个的深拷贝
    for(size_type i = 0 ; i < ht.buckets_.size() ; i++) {
        node_ptr cur = ht.buckets_[i];
        if(cur) {
            node_ptr copy = create_node(cur->value);
            buckets_[i] = copy;     // 第一个node

            for(node_ptr next = cur->next ; next ; cur = next, next = next->next) {
                copy->next = create_node(next->value);
                copy = copy->next;
            }
        }
    }
    num_elems_ = ht.num_elems_;
}



} // !mystl

#endif