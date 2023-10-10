#ifndef __MAP_H__
#define __MAP_H__

#include <initializer_list>

#include "rb_tree.h"
#include "functional.h"

namespace mystl {

// 参数一代表key的类型，参数二代表value的类型， <key, value>，与rb_tree的value不一样
template<class Key, class T, class Compare = mystl::less<Key>>
class map {
public:
    typedef     Key                         key_type;
    typedef     T                           mapped_type;    // 被映射的量
    typedef     mystl::pair</* const  */Key, T>   value_type;
    typedef     Compare                     key_compare;

    // 定义一个内部functor，来进行pair的比较
    class value_compare : public binary_function<value_type, value_type, bool> {
        friend class map<Key, T, Compare>;
        private:
            Compare comp;
            value_compare(Compare c) : comp(c) {}
        public:
            bool operator()(const value_type& lhs, const value_type& rhs) const {
                return comp(lhs.first, rhs.first);      //comp 是key的比较，pair的key为first
            }
    };

private:
    // 红黑树的keyOfValue为select1st
    typedef mystl::rb_tree<key_type, value_type, key_compare, mystl::select1st<value_type>> base_type; 
    base_type  tree_;

public:
    // 使用 rb_tree 的型别
    typedef typename base_type::node_type              node_type;
    typedef typename base_type::pointer                pointer;
    typedef typename base_type::const_pointer          const_pointer;
    typedef typename base_type::reference              reference;
    typedef typename base_type::const_reference        const_reference;
    typedef typename base_type::iterator               iterator;
    typedef typename base_type::const_iterator         const_iterator;
    typedef typename base_type::reverse_iterator       reverse_iterator;
    typedef typename base_type::const_reverse_iterator const_reverse_iterator;
    typedef typename base_type::size_type              size_type;
    typedef typename base_type::difference_type        difference_type;
    typedef typename base_type::allocator_type         allocator_type;

public:
    map() = default;

    template <class InputIterator>
    map(InputIterator first, InputIterator last) : tree_() {
        tree_.insert_unique(first, last);
    }

    map(std::initializer_list<value_type> ilist) : tree_() {
        tree_.insert_unique(ilist.begin(), ilist.end());
    }

    map(const map& rhs) : tree_(rhs.tree_) {}

    map(map&& rhs) : tree_(mystl::move(rhs.tree_)) {}

    map& operator=(const map& rhs) {
        tree_ = rhs.tree_;
        return *this;
    }

    map& operator=(std::initializer_list<value_type> ilist) {
        tree_.clear();
        tree_.insert_unique(ilist.begin(), ilist.end());
        return *this;
    }

    // 相关接口
    key_compare            key_comp()      const { return tree_.key_comp(); }
    value_compare          value_comp()    const { return value_compare(tree_.key_comp()); }
    allocator_type         get_allocator() const { return tree_.get_allocator(); }

    // 迭代器
    iterator               begin()          { return tree_.begin(); }
    const_iterator         begin()   const  { return tree_.begin(); }
    iterator               end()            { return tree_.end(); }
    const_iterator         end()     const  { return tree_.end(); }

    reverse_iterator       rbegin()         { return reverse_iterator(end()); }
    const_reverse_iterator rbegin()  const  { return const_reverse_iterator(end()); }
    reverse_iterator       rend()           { return reverse_iterator(begin()); }
    const_reverse_iterator rend()    const  { return const_reverse_iterator(begin()); }

    const_iterator         cbegin()  const  { return begin(); }
    const_iterator         cend()    const  { return end(); }
    const_reverse_iterator crbegin() const  { return rbegin(); }
    const_reverse_iterator crend()   const  { return rend(); }

    // 容量相关
    bool        empty()     const   { return tree_.empty(); }
    size_type   size()      const   { return tree_.size(); }
    size_type   max_size()  const   { return tree_.max_size();}
    
    // 若键值不存在，抛出异常
    mapped_type&    at(const key_type& key) {
        iterator it = tree_.lower_bound(key);
        // 没有找到的话
        THROW_OUT_OF_RANGE_IF((it == end() || key_comp()(it->first, key)), "map no such elements exists.");
        return it->second;
    }

    const mapped_type&    at(const key_type& key) const {
        iterator it = tree_.lower_bound(key);
        // 没有找到的话
        THROW_OUT_OF_RANGE_IF((it == end() || key_comp()(it->first, key)), "map no such elements exists.");
        return it->second;
    }

    // map提供operator[], 如果不存在元素，插入默认值的mapped_type
    mapped_type&    operator[] (const key_type& key) {
        iterator it = tree_.lower_bound(key);
        if(it == end() || key_comp()(key, it->first)) {
            // 没有则插入
            it = tree_.insert_unique(value_type(key, mapped_type())).first;
        }
        return it->second;
    }

    const mapped_type&    operator[] (const key_type& key) const {
        iterator it = tree_.lower_bound(key);
        if(it == end() || key_comp()(key, it->first)) {
            it = tree_.insert_unique(value_type(key, mapped_type())).first;
        }
        return it->second;
    }

    // insert
    mystl::pair<iterator, bool> insert(const value_type& value) {
        return tree_.insert_unique(value);
    }

    iterator insert(iterator pos, const value_type& value) {
        return tree_.insert_unique(pos, value);
    }

    template <class InputIterator>
    void insert(InputIterator first, InputIterator last) {
        tree_.insert_unique(first, last);
    }

    // erase
    iterator erase(iterator pos) {
        return tree_.erase(pos);
    } 

    size_type erase(const key_type& key) {
        return tree_.erase(key);
    }

    void erase(iterator first, iterator last) {
        tree_.erase(first, last);
    }

    void clear() { tree_.clear(); }

    // map相关操作
    iterator find(const key_type& key) { return tree_.find(key); }
    const_iterator find(const key_type& key) const { return tree_.find(key); }

    size_type count(const key_type& key) { return tree_.count(key); }

    iterator       lower_bound(const key_type& key)       { return tree_.lower_bound(key); }
    const_iterator lower_bound(const key_type& key) const { return tree_.lower_bound(key); }

    iterator       upper_bound(const key_type& key)       { return tree_.upper_bound(key); }
    const_iterator upper_bound(const key_type& key) const { return tree_.upper_bound(key); }

    mystl::pair<iterator, iterator>
    equal_range(const key_type& key) {
        return tree_.equal_range(key);
    }
    
    void swap(map& rhs) {
        tree_.swap(rhs.tree_);
    }
};

// 重载比较操作符
template <class Key, class T, class Compare>
bool operator==(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs)
{
  return lhs == rhs;
}

template <class Key, class T, class Compare>
bool operator<(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs)
{
  return lhs < rhs;
}

template <class Key, class T, class Compare>
bool operator!=(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs)
{
  return !(lhs == rhs);
}

template <class Key, class T, class Compare>
bool operator>(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs)
{
  return rhs < lhs;
}

template <class Key, class T, class Compare>
bool operator<=(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs)
{
  return !(rhs < lhs);
}

template <class Key, class T, class Compare>
bool operator>=(const map<Key, T, Compare>& lhs, const map<Key, T, Compare>& rhs)
{
  return !(lhs < rhs);
}


// multimap
template<class Key, class T, class Compare = mystl::less<Key>>
class multimap {
public:
    typedef     Key                         key_type;
    typedef     T                           mapped_type;    // 被映射的量
    typedef     mystl::pair<const Key, T>   value_type;
    typedef     Compare                     key_compare;

    // 定义一个内部functor，来进行pair的比较
    class value_compare : public binary_function<value_type, value_type, bool> {
        friend class multimap<Key, T, Compare>;
        private:
            Compare comp;
            value_compare(Compare c) : comp(c) {}
        public:
            bool operator()(const value_type& lhs, const value_type& rhs) const {
                return comp(lhs.first, rhs.first);      //comp 是key的比较，pair的key为first
            }
    };

private:
    // 红黑树的keyOfValue为select1st
    typedef mystl::rb_tree<key_type, value_type, key_compare, mystl::select1st<value_type>> base_type; 
    base_type  tree_;

public:
    // 使用 rb_tree 的型别
    typedef typename base_type::node_type              node_type;
    typedef typename base_type::pointer                pointer;
    typedef typename base_type::const_pointer          const_pointer;
    typedef typename base_type::reference              reference;
    typedef typename base_type::const_reference        const_reference;
    typedef typename base_type::iterator               iterator;
    typedef typename base_type::const_iterator         const_iterator;
    typedef typename base_type::reverse_iterator       reverse_iterator;
    typedef typename base_type::const_reverse_iterator const_reverse_iterator;
    typedef typename base_type::size_type              size_type;
    typedef typename base_type::difference_type        difference_type;
    typedef typename base_type::allocator_type         allocator_type;

public:
    multimap() = default;

    template <class InputIterator>
    multimap(InputIterator first, InputIterator last) : tree_() {
        tree_.insert_equal(first, last);
    }

    multimap(std::initializer_list<value_type> ilist) : tree_() {
        tree_.insert_equal(ilist.begin(), ilist.end());
    }

    multimap(const multimap& rhs) : tree_(rhs.tree_) {}

    multimap(multimap&& rhs) : tree_(mystl::move(rhs.tree_)) {}

    multimap& operator=(const multimap& rhs) {
        tree_ = rhs.tree_;
        return *this;
    }

    multimap& operator=(std::initializer_list<value_type> ilist) {
        tree_.clear();
        tree_.insert_equal(ilist.begin(), ilist.end());
        return *this;
    }

    // 相关接口
    key_compare            key_comp()      const { return tree_.key_comp(); }
    value_compare          value_comp()    const { return value_compare(tree_.key_comp()); }
    allocator_type         get_allocator() const { return tree_.get_allocator(); }

    // 迭代器
    iterator               begin()          { return tree_.begin(); }
    const_iterator         begin()   const  { return tree_.begin(); }
    iterator               end()            { return tree_.end(); }
    const_iterator         end()     const  { return tree_.end(); }

    reverse_iterator       rbegin()         { return reverse_iterator(end()); }
    const_reverse_iterator rbegin()  const  { return const_reverse_iterator(end()); }
    reverse_iterator       rend()           { return reverse_iterator(begin()); }
    const_reverse_iterator rend()    const  { return const_reverse_iterator(begin()); }

    const_iterator         cbegin()  const  { return begin(); }
    const_iterator         cend()    const  { return end(); }
    const_reverse_iterator crbegin() const  { return rbegin(); }
    const_reverse_iterator crend()   const  { return rend(); }

    // 容量相关
    bool        empty()     const   { return tree_.empty(); }
    size_type   size()      const   { return tree_.size(); }
    size_type   max_size()  const   { return tree_.max_size();}
    
    // 若键值不存在，抛出异常
    mapped_type&    at(const key_type& key) {
        iterator it = tree_.lower_bound(key);
        // 没有找到的话
        THROW_OUT_OF_RANGE_IF(it == end() || key_comp(it->first, key), "multimap no such elements exists.");
        return it->second;
    }

    const mapped_type&    at(const key_type& key) const {
        iterator it = tree_.lower_bound(key);
        // 没有找到的话
        THROW_OUT_OF_RANGE_IF(it == end() || key_comp(it->first, key), "multimap no such elements exists.");
        return it->second;
    }

    // multimap提供operator[], 如果不存在元素，插入默认值的mapped_type
    mapped_type&    operator[] (const key_type& key) {
        iterator it = tree_.lower_bound(key);
        if(it == end() || key_comp(it->first, key)) {
            // 没有则插入
            it = tree_.insert_equal(value_type(key, mapped_type())).first;
        }
        return it->second;
    }

    const mapped_type&    operator[] (const key_type& key) const {
        iterator it = tree_.lower_bound(key);
        if(it == end() || key_comp(it->first, key)) {
            it = tree_.insert_equal(value_type(key, mapped_type())).first;
        }
        return it->second;
    }

    // insert
    iterator insert(const value_type& value) {
        return tree_.insert_equal(value);
    }

    iterator insert(iterator pos, const value_type& value) {
        return tree_.insert_equal(pos, value);
    }

    template <class InputIterator>
    void insert(InputIterator first, InputIterator last) {
        tree_.insert_equal(first, last);
    }

    // erase
    iterator erase(iterator pos) {
        return tree_.erase(pos);
    } 

    size_type erase(const key_type& key) {
        return tree_.erase(key);
    }

    void erase(iterator first, iterator last) {
        tree_.erase(first, last);
    }

    void clear() { tree_.clear(); }

    // multimap相关操作
    iterator find(const key_type& key) { return tree_.find(key); }
    const_iterator find(const key_type& key) const { return tree_.find(key); }

    size_type count(const key_type& key) { return tree_.count(key); }

    iterator       lower_bound(const key_type& key)       { return tree_.lower_bound(key); }
    const_iterator lower_bound(const key_type& key) const { return tree_.lower_bound(key); }

    iterator       upper_bound(const key_type& key)       { return tree_.upper_bound(key); }
    const_iterator upper_bound(const key_type& key) const { return tree_.upper_bound(key); }

    mystl::pair<iterator, iterator>
    equal_range(const key_type& key) {
        return tree_.equal_range(key);
    }
    
    void swap(multimap& rhs) {
        tree_.swap(rhs.tree_);
    }
};

// 重载比较操作符
template <class Key, class T, class Compare>
bool operator==(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs)
{
  return lhs == rhs;
}

template <class Key, class T, class Compare>
bool operator<(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs)
{
  return lhs < rhs;
}

template <class Key, class T, class Compare>
bool operator!=(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs)
{
  return !(lhs == rhs);
}

template <class Key, class T, class Compare>
bool operator>(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs)
{
  return rhs < lhs;
}

template <class Key, class T, class Compare>
bool operator<=(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs)
{
  return !(rhs < lhs);
}

template <class Key, class T, class Compare>
bool operator>=(const multimap<Key, T, Compare>& lhs, const multimap<Key, T, Compare>& rhs)
{
  return !(lhs < rhs);
}


}

#endif