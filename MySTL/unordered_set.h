#ifndef __UNORDERED_SET_H__
#define __UNORDERED_SET_H__

#include "hashtable.h"

namespace mystl {

template <class Key, class Hash = mystl::hash<Key>, class KeyEqual = mystl::equal_to<Key>>
class unordered_set {
private:
    // 用一个哈希表作为成员，KeyofValue为identity
    typedef hashtable<Key, Key, Hash, mystl::identity<Key>, KeyEqual> base_type;
    base_type   ht_;

public:
    // 几种functor的型别
    typedef typename base_type::allocator_type      allocator_type;
    typedef typename base_type::key_type            key_type;       // Key
    typedef typename base_type::value_type          value_type;     // Key
    typedef typename base_type::hasher              hasher;
    typedef typename base_type::key_equal           key_eqaul;
    typedef mystl::identity<Key>                    get_key;

    typedef typename base_type::size_type           size_type;
    typedef typename base_type::difference_type     difference_type;
    typedef typename base_type::pointer             pointer;
    typedef typename base_type::const_pointer       const_pointer;
    typedef typename base_type::reference           reference;
    typedef typename base_type::const_reference     const_reference;

    typedef typename base_type::iterator            iterator;
    typedef typename base_type::const_iterator      const_iterator;

    allocator_type get_allocator() const { return ht_.get_allocator(); }

public:
    // 初始就用53的容量
    unordered_set() : ht_() {} // 采用默认构造即可，hash、equal、get_key已经传入了

    unordered_set(size_type bucket_count, 
                 const Hash& hash = Hash(),
                 const KeyEqual& equal = KeyEqual())
    : ht_(bucket_count, hash, equal, get_key()) { }

    template <class Iterator>
    unordered_set(Iterator first, Iterator last,
                  const size_type& bucket_count = 53,
                  const Hash& hash = Hash(),
                  const KeyEqual& equal = KeyEqual())
    : ht_(mystl::max(bucket_count, static_cast<size_type>(mystl::distance(first, last))),
          hash, equal, get_key()) {
        for(; first != last ; ++first) {
            ht_.insert_unique(*first);
        }
    }

    unordered_set(std::initializer_list<Key> ilist,
                  const size_type& bucket_count = 53,
                  const Hash& hash = Hash(),
                  const KeyEqual& equal = KeyEqual())
    : ht_(mystl::max(bucket_count, static_cast<size_type>(mystl::distance(ilist.begin(), ilist.end()))),
          hash, equal, get_key()) {
        for(auto first = ilist.begin() ; first != ilist.end() ; ++first) {
            ht_.insert_unique(*first);
        }
    }

    unordered_set(const unordered_set& rhs) : ht_(rhs.ht_) { }

    unordered_set(unordered_set&& rhs) : ht_(mystl::move(rhs.ht_)) { }

    unordered_set& operator=(const unordered_set& rhs) {
        ht_ = rhs.ht_;
        return *this;
    }

    unordered_set& operator=(unordered_set&& rhs) {
        ht_ = mystl::move(rhs.ht_);
        return *this;
    }

    // hashtable没有做init_list的operator=
    unordered_set& operator=(std::initializer_list<value_type> ilist) {
        ht_.clear();
        for(auto first = ilist.begin() ; first != ilist.end() ; ++first) {
            ht_.insert_unique(*first);
        }
        return *this;
    }
    
    ~unordered_set() = default;

public:
    // 迭代器相关
    iterator        begin()             { return ht_.begin(); }
    const_iterator  begin()     const   { return ht_.begin(); }
    iterator        end()               { return ht_.end(); }
    const_iterator  end()       const   { return ht_.end(); }

    const_iterator  cbegin()    const   { return ht_.begin(); }
    const_iterator  cend()      const   { return ht_.end(); }

    // 容量相关
    bool        empty()     const { return ht_.empty(); }
    size_type   size()      const { return ht_.size(); }
    size_type   max_size()  const { return ht_.max_size(); }


    // insert <iterator, bool> 因为不允许重复
    pair<iterator, bool> insert(const value_type& value) {
        return ht_.insert_unique(value);
    }

    template <class Iterator>
    void insert(Iterator first, Iterator last) {
        ht_.insert_unique(first, last);
    }


    // erase
    size_type erase(const key_type& key) {
        return ht_.erase(key);
    }
    
    iterator erase(const iterator& it) {
        return ht_.erase(it);
    }

    void erase(iterator first, iterator last) {
        ht_.erase(first, last); 
    }

    iterator find(const key_type& key) {
        return ht_.find(key);
    }

    size_type count(const key_type& key) const {
        return ht_.count(key);
    }

    pair<iterator, iterator> equal_range(const key_type& key) {
        return ht_.equal_range(key);
    }  

    void swap(unordered_set& other) {
        ht_.swap(other.ht_);
    }

    void clear() { ht_.clear(); }

public:
    // 关于buckets的函数

    // 这个仅仅是对buckets扩容
    void resize(size_type hint) { ht_.resize(); }
    size_type bucket_count() const { return ht_.bucket_count(); }
    size_type max_bucket_count() const { return ht_.max_bucket_count(); }
    size_type elems_in_bucket(size_type bucket_idx) const {
        return ht_.elems_in_bucket(bucket_idx);
    }
};
// hashtable不提供operator==，因为是无序关联式容器
// 重载swap
template <class Key, class Hash, class KeyEqual>
void swap(unordered_set<Key, Hash, KeyEqual>& lhs, 
          unordered_set<Key, Hash, KeyEqual>& rhs) {
    lhs.swap(rhs);
}


// unordered_multiset 区别就是在与insert_equal

template <class Key, class Hash = mystl::hash<Key>, class KeyEqual = mystl::equal_to<Key>>
class unordered_multiset {
private:
    // 用一个哈希表作为成员，KeyofValue为identity
    typedef hashtable<Key, Key, Hash, mystl::identity<Key>, KeyEqual> base_type;
    base_type   ht_;

public:
    // 几种functor的型别
    typedef typename base_type::allocator_type      allocator_type;
    typedef typename base_type::key_type            key_type;       // Key
    typedef typename base_type::value_type          value_type;     // Key
    typedef typename base_type::hasher              hasher;
    typedef typename base_type::key_equal           key_eqaul;
    typedef mystl::identity<Key>                    get_key;

    typedef typename base_type::size_type           size_type;
    typedef typename base_type::difference_type     difference_type;
    typedef typename base_type::pointer             pointer;
    typedef typename base_type::const_pointer       const_pointer;
    typedef typename base_type::reference           reference;
    typedef typename base_type::const_reference     const_reference;

    typedef typename base_type::iterator            iterator;
    typedef typename base_type::const_iterator      const_iterator;

    allocator_type get_allocator() const { return ht_.get_allocator(); }

public:
    // 初始就用53的容量
    unordered_multiset() : ht_() {} // 采用默认构造即可，hash、equal、get_key已经传入了

    unordered_multiset(size_type bucket_count, 
                 const Hash& hash = Hash(),
                 const KeyEqual& equal = KeyEqual())
    : ht_(bucket_count, hash, equal, get_key()) { }

    template <class Iterator>
    unordered_multiset(Iterator first, Iterator last,
                  const size_type& bucket_count = 53,
                  const Hash& hash = Hash(),
                  const KeyEqual& equal = KeyEqual())
    : ht_(mystl::max(bucket_count, static_cast<size_type>(mystl::distance(first, last))),
          hash, equal, get_key()) {
        for(; first != last ; ++first) {
            ht_.insert_unique(*first);
        }
    }

    unordered_multiset(std::initializer_list<Key> ilist,
                  const size_type& bucket_count = 53,
                  const Hash& hash = Hash(),
                  const KeyEqual& equal = KeyEqual())
    : ht_(mystl::max(bucket_count, static_cast<size_type>(mystl::distance(ilist.begin(), ilist.end()))),
          hash, equal, get_key()) {
        for(auto first = ilist.begin() ; first != ilist.end() ; ++first) {
            ht_.insert_unique(*first);
        }
    }

    unordered_multiset(const unordered_multiset& rhs) : ht_(rhs.ht_) { }

    unordered_multiset(unordered_multiset&& rhs) : ht_(mystl::move(rhs.ht_)) { }

    unordered_multiset& operator=(const unordered_multiset& rhs) {
        ht_ = rhs.ht;
        return *this;
    }

    unordered_multiset& operator=(unordered_multiset&& rhs) {
        ht_ = mystl::move(rhs.ht_);
        return *this;
    }

    // hashtable没有做init_list的operator=
    unordered_multiset& operator=(std::initializer_list<value_type> ilist) {
        ht_.clear();
        for(auto first = ilist.begin() ; first != ilist.end() ; ++first) {
            ht_.insert_unique(*first);
        }
        return *this;
    }
    
    ~unordered_multiset() = default;

public:
    // 迭代器相关
    iterator        begin()             { return ht_.begin(); }
    const_iterator  begin()     const   { return ht_.begin(); }
    iterator        end()               { return ht_.end(); }
    const_iterator  end()       const   { return ht_.end(); }

    const_iterator  cbegin()    const   { return ht_.begin(); }
    const_iterator  cend()      const   { return ht_.end(); }

    // 容量相关
    bool        empty()     const { return ht_.empty(); }
    size_type   size()      const { return ht_.size(); }
    size_type   max_size()  const { return ht_.max_size(); }


    // insert
    iterator insert(const value_type& value) {
        return ht_.insert_equal(value);
    }

    template <class Iterator>
    void insert(Iterator first, Iterator last) {
        ht_.insert_equal(first, last);
    }


    // erase
    size_type erase(const key_type& key) {
        return ht_.erase(key);
    }
    
    iterator erase(const iterator& it) {
        return ht_.erase(it);
    }

    void erase(iterator first, iterator last) {
        ht_.erase(first, last); 
    }

    iterator find(const key_type& key) {
        return ht_.find(key);
    }

    size_type count(const key_type& key) const {
        return ht_.count(key);
    }

    pair<iterator, iterator> equal_range(const key_type& key) const {
        return ht_.equal_range(key);
    }

    void swap(unordered_multiset& other) {
        ht_.swap(other.ht_);
    }

    void clear() { ht_.clear(); }

public:
    // 关于buckets的函数

    // 这个仅仅是对buckets扩容
    void resize(size_type hint) { ht_.resize(); }
    size_type bucket_count() const { return ht_.bucket_count(); }
    size_type max_bucket_count() const { return ht_.max_bucket_count(); }
    size_type elems_in_bucket(size_type bucket_idx) const {
        return ht_.elems_in_bucket(bucket_idx);
    }
};

// 重载swap
template <class Key, class Hash, class KeyEqual>
void swap(unordered_multiset<Key, Hash, KeyEqual>& lhs, 
          unordered_multiset<Key, Hash, KeyEqual>& rhs) {
    lhs.swap(rhs);
}

}


#endif