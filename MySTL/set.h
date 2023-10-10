#ifndef __SET_H__
#define __SET_H__

#include <initializer_list>

#include "rb_tree.h"


namespace mystl {

template <class Key, class Compare = mystl::less<Key>>
class set {
public:
    typedef Key         key_type;
    typedef Key         value_type; // key == value
    typedef Compare     key_compare;
    typedef Compare     value_compare;

private:
    // 内部含有红黑树, 采用identity作为KeyOfValue
    typedef mystl::rb_tree<key_type, value_type, key_compare, mystl::identity<value_type>>  base_type;
    base_type tree_;

public:
    // 类型全部来自底层红黑树
    typedef typename base_type::node_type              node_type;
    typedef typename base_type::const_pointer          pointer;
    typedef typename base_type::const_pointer          const_pointer;
    typedef typename base_type::const_reference        reference;
    typedef typename base_type::const_reference        const_reference;
    typedef typename base_type::const_iterator         iterator;
    typedef typename base_type::const_iterator         const_iterator;
    typedef typename base_type::const_reverse_iterator reverse_iterator;
    typedef typename base_type::const_reverse_iterator const_reverse_iterator;
    typedef typename base_type::size_type              size_type;
    typedef typename base_type::difference_type        difference_type;
    typedef typename base_type::allocator_type         allocator_type;

public:
    set() = default;

    template <class InputIterator>
    set(InputIterator first, InputIterator last) : tree_() {
        tree_.insert_unique(first, last);
    }

    set(std::initializer_list<value_type> ilist) {
        tree_.insert_unique(ilist.begin(), ilist.end());
    }

    set(const set& rhs) : tree_(rhs.tree_) { }

    set(set&& rhs) : tree_(mystl::move(rhs.tree_)) { }

    set& operator=(const set& rhs) {
        tree_ = rhs.tree_;
        return *this;
    }

    set& operator=(set&& rhs) {
        tree_ = mystl::move(rhs.tree_);
        return *this;
    }

    set& operator=(std::initializer_list<value_type> ilist) {
        tree_.clear();
        tree_.insert_unique(ilist.begin(), ilist.end());
        return *this;
    }

    // 一些类型返回
    key_compare         key_comp()      const { return tree_.key_comp_(); }
    value_compare       value_comp()    const { return tree_.key_comp_(); }
    allocator_type      get_allocator() const { return tree_.get_allocator(); } 

    // 迭代器
    iterator               begin()          { return tree_.begin(); }
    const_iterator         begin()  const   { return tree_.begin(); }
    iterator               end()            { return tree_.end(); }
    const_iterator         end()    const   { return tree_.end(); }

    reverse_iterator       rbegin()         { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const   { return const_reverse_iterator(end()); }
    reverse_iterator       rend()           { return reverse_iterator(begin()); }
    const_reverse_iterator rend()   const   { return const_reverse_iterator(begin()); }

    const_iterator         cbegin()     const       { return begin(); }
    const_iterator         cend()       const       { return end(); }
    const_reverse_iterator crbegin()    const       { return rbegin(); }
    const_reverse_iterator crend()      const       { return rend(); }

    // 容量
    bool        empty()     const { return tree_.empty(); }
    size_type   size()      const { return tree_.size();  }
    size_type   max_size()    const { return tree_.max_size();}

    // insert
    mystl::pair<iterator, bool> insert(const value_type& value) {
        return tree_.insert_unique(value);
    }
    iterator insert(iterator pos, const value_type& value) {
        return tree_.insert_unique(pos, value);
    }
    template <class Iterator>
    void insert(Iterator first, Iterator last) {
        tree_.insert_unique(first, last);
    }

    // erase
    iterator erase(iterator pos) { return tree_.erase(pos); } 
    size_type erase(const key_type& key) { return tree_.erase(key); }
    void erase(iterator first, iterator last) { tree_.erase(first, last); }

    // clear
    void clear() { tree_.clear(); }

    // set相关的操作
    iterator find(const key_type& key)  { return tree_.find(key); }
    const_iterator find(const key_type& key)    const { return tree_.find(key); }

    size_type count(const key_type& key) const { return tree_.count(key); }

    iterator       lower_bound(const key_type& key)       { return tree_.lower_bound(key); }
    const_iterator lower_bound(const key_type& key) const { return tree_.lower_bound(key); }

    iterator       upper_bound(const key_type& key)       { return tree_.upper_bound(key); }
    const_iterator upper_bound(const key_type& key) const { return tree_.upper_bound(key); }

    mystl::pair<iterator, iterator>
    equal_range(const key_type& key) {
        return tree_.equal_range(key);
    }

    void swap(set& rhs) {
        tree_.swap(rhs.tree_);
    }
};

template <class Key, class Compare>
bool operator==(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
{
  return lhs == rhs;
}

template <class Key, class Compare>
bool operator<(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
{
  return lhs < rhs;
}

template <class Key, class Compare>
bool operator!=(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
{
  return !(lhs == rhs);
}

template <class Key, class Compare>
bool operator>(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
{
  return rhs < lhs;
}

template <class Key, class Compare>
bool operator<=(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
{
  return !(rhs < lhs);
}

template <class Key, class Compare>
bool operator>=(const set<Key, Compare>& lhs, const set<Key, Compare>& rhs)
{
  return !(lhs < rhs);
}

// 重载 mystl 的 swap
template <class Key, class Compare>
void swap(set<Key, Compare>& lhs, set<Key, Compare>& rhs) noexcept
{
  lhs.swap(rhs);
}



//--------------------multiset-----------------------
template <class Key, class Compare = mystl::less<Key>>
class multiset {
public:
    typedef Key         key_type;
    typedef Key         value_type; // key == value
    typedef Compare     key_compare;
    typedef Compare     value_compare;

private:
    // 内部含有红黑树, 采用identity作为KeyOfValue
    typedef mystl::rb_tree<key_type, value_type, key_compare, mystl::identity<value_type>>  base_type;
    base_type tree_;

public:
    // 类型全部来自底层红黑树
    typedef typename base_type::node_type              node_type;
    typedef typename base_type::const_pointer          pointer;
    typedef typename base_type::const_pointer          const_pointer;
    typedef typename base_type::const_reference        reference;
    typedef typename base_type::const_reference        const_reference;
    typedef typename base_type::const_iterator         iterator;
    typedef typename base_type::const_iterator         const_iterator;
    typedef typename base_type::const_reverse_iterator reverse_iterator;
    typedef typename base_type::const_reverse_iterator const_reverse_iterator;
    typedef typename base_type::size_type              size_type;
    typedef typename base_type::difference_type        difference_type;
    typedef typename base_type::allocator_type         allocator_type;

public:
    multiset() = default;

    template <class InputIterator>
    multiset(InputIterator first, InputIterator last) : tree_() {
        tree_.insert_equal(first, last);
    }

    multiset(std::initializer_list<value_type> ilist) {
        tree_.insert_equal(ilist.begin(), ilist.end());
    }

    multiset(const multiset& rhs) : tree_(rhs.tree_) { }

    multiset(multiset&& rhs) : tree_(mystl::move(rhs.tree_)) { }

    multiset& operator=(const multiset& rhs) {
        tree_ = rhs.tree_;
        return *this;
    }

    multiset& operator=(multiset&& rhs) {
        tree_ = mystl::move(rhs.tree_);
        return *this;
    }

    multiset& operator=(std::initializer_list<value_type> ilist) {
        tree_.clear();
        tree_.insert_equal(ilist.begin(), ilist.end());
        return *this;
    }

    // 一些类型返回
    key_compare         key_comp()      const { return tree_.key_comp_(); }
    value_compare       value_comp()    const { return tree_.key_comp_(); }
    allocator_type      get_allocator() const { return tree_.get_allocator(); } 

    // 迭代器
    iterator               begin()          { return tree_.begin(); }
    const_iterator         begin()  const   { return tree_.begin(); }
    iterator               end()            { return tree_.end(); }
    const_iterator         end()    const   { return tree_.end(); }

    reverse_iterator       rbegin()         { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const   { return const_reverse_iterator(end()); }
    reverse_iterator       rend()           { return reverse_iterator(begin()); }
    const_reverse_iterator rend()   const   { return const_reverse_iterator(begin()); }

    const_iterator         cbegin()     const       { return begin(); }
    const_iterator         cend()       const       { return end(); }
    const_reverse_iterator crbegin()    const       { return rbegin(); }
    const_reverse_iterator crend()      const       { return rend(); }

    // 容量
    bool        empty()     const { return tree_.empty(); }
    size_type   size()      const { return tree_.size();  }
    size_type   max_size()    const { return tree_.max_size();}

    // insert
    mystl::pair<iterator, bool> insert(const value_type& value) {
        return tree_.insert_equale(value);
    }
    iterator insert(iterator pos, const value_type& value) {
        return tree_.insert_equale(pos, value);
    }
    template <class Iterator>
    void insert(Iterator first, Iterator last) {
        tree_.insert_equal(first, last);
    }

    // erase
    iterator erase(iterator pos) { return tree_.erase(pos); } 
    size_type erase(const key_type& key) { return tree_.erase(key); }
    void erase(iterator first, iterator last) { tree_.erase(first, last); }

    // clear
    void clear() { tree_.clear(); }

    // set相关的操作
    iterator find(const key_type& key)  { return tree_.find(key); }
    const_iterator find(const key_type& key)    const { return tree_.find(key); }

    size_type count(const key_type& key) const { return tree_.count(key); }

    iterator       lower_bound(const key_type& key)       { return tree_.lower_bound(key); }
    const_iterator lower_bound(const key_type& key) const { return tree_.lower_bound(key); }

    iterator       upper_bound(const key_type& key)       { return tree_.upper_bound(key); }
    const_iterator upper_bound(const key_type& key) const { return tree_.upper_bound(key); }

    mystl::pair<iterator, iterator>
    equal_range(const key_type& key) {
        return tree_.equal_range(key);
    }

    void swap(multiset& rhs) {
        tree_.swap(rhs.tree_);
    }
};

template <class Key, class Compare>
bool operator==(const multiset<Key, Compare>& lhs, const multiset<Key, Compare>& rhs)
{
  return lhs == rhs;
}

template <class Key, class Compare>
bool operator<(const multiset<Key, Compare>& lhs, const multiset<Key, Compare>& rhs)
{
  return lhs < rhs;
}

template <class Key, class Compare>
bool operator!=(const multiset<Key, Compare>& lhs, const multiset<Key, Compare>& rhs)
{
  return !(lhs == rhs);
}

template <class Key, class Compare>
bool operator>(const multiset<Key, Compare>& lhs, const multiset<Key, Compare>& rhs)
{
  return rhs < lhs;
}

template <class Key, class Compare>
bool operator<=(const multiset<Key, Compare>& lhs, const multiset<Key, Compare>& rhs)
{
  return !(rhs < lhs);
}

template <class Key, class Compare>
bool operator>=(const multiset<Key, Compare>& lhs, const multiset<Key, Compare>& rhs)
{
  return !(lhs < rhs);
}

// 重载 mystl 的 swap
template <class Key, class Compare>
void swap(multiset<Key, Compare>& lhs, multiset<Key, Compare>& rhs) noexcept
{
  lhs.swap(rhs);
}


}

#endif