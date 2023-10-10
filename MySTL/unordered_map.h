#ifndef __UNORDERED_MAP_H__
#define __UNORDERED_MAP_H__

#include "hashtable.h"
#include "functional.h"

namespace mystl {

// 第一个参数为key，第二个参数为value，第三个参数为哈希函数，第四个参数为判断key是否相等
template <class Key, class T, class Hash = mystl::hash<Key>, class KeyEqual = mystl::equal_to<Key>>
class unordered_map {
private:
    typedef hashtable<Key, mystl::pair</* const */Key, T>, Hash, 
                    mystl::select1st<mystl::pair</* const */Key, T>>, KeyEqual> base_type;
    base_type ht_;

public:
    typedef typename base_type::allocator_type       allocator_type;
    typedef typename base_type::key_type             key_type;
    typedef          T                               mapped_type;
    typedef typename base_type::value_type           value_type;
    typedef typename base_type::hasher               hasher;
    typedef typename base_type::key_equal            key_equal;
    typedef mystl::select1st<mystl::pair</* const */Key, T>>    get_key;

    typedef typename base_type::size_type            size_type;
    typedef typename base_type::difference_type      difference_type;
    typedef typename base_type::pointer              pointer;
    typedef typename base_type::const_pointer        const_pointer;
    typedef typename base_type::reference            reference;
    typedef typename base_type::const_reference      const_reference;

    typedef typename base_type::iterator             iterator;
    typedef typename base_type::const_iterator       const_iterator;

    allocator_type get_allocator() const { return ht_.get_allocator(); }

public:
    unordered_map() : ht_() { }

    unordered_map(size_type bucket_count,
                  const Hash& hash = Hash(),
                  const KeyEqual& equal = KeyEqual(),
                  const get_key& ex_key = get_key()) 
    : ht_(bucket_count, hash, equal, ex_key)            
    { }

    template <class Iterator>
    unordered_map(Iterator first, Iterator last,
                  size_type bucket_count = 53,
                  const Hash& hash = Hash(),
                  const KeyEqual& equal = KeyEqual(),
                  const get_key& ex_key = get_key()) 
    : ht_(mystl::max(bucket_count, static_cast<size_type>(mystl::distance(first, last))),
          hash, equal, ex_key )  {
        for(; first != last ; ++first) {
            ht_.insert_unique(*first);
        }
    }

    unordered_map(std::initializer_list<value_type> ilist, 
                  size_type bucket_count = 53,
                  const Hash& hash = Hash(),
                  const KeyEqual& equal = KeyEqual(),
                  const get_key& ex_key = get_key()) 
    : ht_(mystl::max(bucket_count, static_cast<size_type>(mystl::distance(ilist.begin(), ilist.end()))),
          hash, equal, ex_key )  {
        for(auto first = ilist.begin() ; first != ilist.end(); ++first) {
            ht_.insert_unique(*first);
        }
    } 

    unordered_map(const unordered_map& rhs) : ht_(rhs.ht_) { }

    unordered_map(unordered_map&& rhs) : ht_(mystl::move(rhs.ht_)) { }

    unordered_map& operator=(const unordered_map& rhs) {
        ht_ = rhs.ht_;
        return *this;
    }

    unordered_map& operator=(unordered_map&& rhs) {
        ht_ = mystl::move(ht_);
        return *this;
    }

    unordered_map& operator=(std::initializer_list<value_type> ilist) {
        ht_.clear();
        // 不用做预留，会自己扩容哈希表
        /* ht_.buckets_.reserve(ilist.size()); */
        for(auto first = ilist.begin() ; first != ilist.end() ; ++first) {
            ht_.insert_unique(*first);
        }
        return *this;
    }

    ~unordered_map() = default;

public:
    // 迭代器
    iterator        begin()         { return ht_.begin(); }
    const_iterator  begin() const   { return ht_.begin(); }
    iterator        end()           { return ht_.end(); }
    const_iterator  end()   const   { return ht_.end(); }

    const_iterator  cbegin()    const { return ht_.cbegin(); }
    const_iterator  cend()      const { return ht_.cend(); }

    // 容量相关
    bool        empty()     const { return ht_.empty(); }
    size_type   size()      const { return ht_.size(); }
    size_type   max_size()  const { return ht_.max_size(); }

    // insert
    pair<iterator, bool> insert(const value_type& value) {
        return ht_.insert_unique(value);
    } 

    template <class Iterator>
    void insert(Iterator first, Iterator last) {
        ht_.insert_unique(first, last);
    }

    // erase
    iterator erase(iterator it) {
        return ht_.erase(it);
    }

    size_type erase(const key_type& key) {
        return ht_.erase(key);
    }

    void erase(iterator first, iterator last) {
        return ht_.erase(first, last);
    }

    void clear() {
        ht_.clear();
    }

    void swap(unordered_map& other) {
        ht_.swap(other.ht_);    
    }   

    // 查找相关

    mapped_type& at(const key_type& key) {
        iterator it = ht_.find(key);
        THROW_OUT_OF_RANGE_IF(it.cur == nullptr, "unordered_map<Key, T> no such element.\n");
        return it->second;
    }

    const mapped_type& at(const key_type& key) const {
        iterator it = ht_.find(key);
        THROW_OUT_OF_RANGE_IF(it.cur == nullptr, "unordered_map<Key, T> no such element.\n");
        return it->second;
    }

    mapped_type& operator[](const key_type& key) {
        iterator it = ht_.find(key);
        if(it.cur == nullptr) {
            // 插入 默认值
            it = ht_.insert_unique(pair<key_type,mapped_type>(key, mapped_type())).first;
        }
        return it->second;
    }

    mapped_type& operator[](key_type&& key) {
        iterator it = ht_.find(key);
        if(it.cur == nullptr) {
            // 插入
            it = ht_.insert_unique(pair<key_type,mapped_type>(key, mapped_type())).first;
        }
        return it->second;  // value
    }

    size_type count(const key_type& key) const {
        return ht_.count(key);
    }

    iterator find(const key_type& key) {
        return ht_.find(key);
    }

    pair<iterator, iterator> equal_range(const key_type& key) {
        return ht_.equal_range(key);
    }


    // bucket
    void resize(size_type hint) { ht_.resize(); }

    size_type bucket_count() const { return ht_.bucket_count(); }

    size_type max_bucket_count() const { return ht_.max_bucket_count(); }
    
    size_type elems_in_bucket(size_type bucket_idx) const {
        return ht_.elems_in_bucket(bucket_idx);
    }

};

}


#endif