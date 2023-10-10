#ifndef __RB_TREE_H__
#define __RB_TREE_H__

#include <initializer_list>
#include <cassert>

#include "functional.h"
#include "iterator.h"
#include "memory.h"
#include "type_traits.h"
#include "exceptdef.h"
#include "allocator.h"


namespace mystl {

// rb_tree 节点颜色类型 用bool

typedef bool rb_tree_color_type;

static constexpr rb_tree_color_type rb_tree_red = false;
static constexpr rb_tree_color_type rb_tree_black = true;


// rb_tree_node 的设计

// 前向引用
template <class T>
struct rb_tree_node;

template <class T>
struct rb_tree_node_base {
    typedef rb_tree_color_type color_type;
    typedef rb_tree_node_base<T>*   base_ptr;  // node基类指针
    typedef rb_tree_node<T>*        node_ptr;

    base_ptr    parent_;    // 父节点
    base_ptr    left_;      // 左子节点
    base_ptr    right_;     // 右子节点
    color_type  color_;     // 结点颜色

    // 本对象的地址
    base_ptr get_base_ptr() {
        return &*this;
    }

    // reinterpret_cast 用于基类和派生类之间的转化，强类型转化，安全性由用户保证
    node_ptr get_node_ptr() {
        return reinterpret_cast<node_ptr>(&*this);
    }

    node_ptr& get_node_ref() {
        return reinterpret_cast<node_ptr&>(*this);
    }
};

template <class T>
struct rb_tree_node : public rb_tree_node_base<T> {
    typedef rb_tree_node_base<T>*   base_ptr;
    typedef rb_tree_node<T>*        node_ptr;

    T   value_;  // 结点的值

    base_ptr get_base_ptr() {
        return static_cast<base_ptr>(&*this);
    }

    node_ptr get_node_ptr() {
        return &*this;
    }
};


// rb_tree_iterator 的设计，内含有base_ptr本身
template <class T>
struct rb_tree_iterator_base {
    typedef rb_tree_node_base<T>* base_ptr;
    typedef rb_tree_node<T>*      node_ptr;
    typedef bidirectional_iterator_tag iterator_category;
    typedef ptrdiff_t difference_type;

    base_ptr node_;     // node_ 指向rb_tree_node的指针，与node结点产生联系

    // 使迭代器前进一个 (在BST排序的基础上前进)
    void increment() {
        if(node_->right_ != nullptr) {
            // 如果右节点不为空，则走到最右结点的最左边
            node_ = node_->right_;
            while(node_->left_ != nullptr) {
                node_ = node_->left_;
            }
        }else {
            // 右节点为空，说明当前子树已经被访问完，需要上溯到，node不为右节点为止
            base_ptr y = node_->parent_;
            while(node_ == y->right_) {
                node_ = y;
                y = y->parent_;
            }
            
            // 这里是防止node走到header，如果为header返回的就是node本身
            // 当node本身指向根节点，且无右子节点，此时会走到header，如果不加以判断，则会又返回自己本身
            // 如果这样的话，node就是head，也就是迭代器end()，此时root充当最后一个元素
            if(node_->right_ != y) {
                // 找到y->left = node，没有特殊情况，y就是下一个node
                node_ = y;
            }
        }
    }

    // 使迭代器后退
    void decrement() {
        // node为header，header为红色的，root为黑色，两者区别在这里，后一个条件都满足
        if(node_->color_ == rb_tree_red && node_->parent_->parent_ == node_) {
            node_ = node_->right_;  //head->right连接最大的值
        }else if(node_->left_ != nullptr) {
            // 找左子树的最右节点
            node_ = node_->left_;
            while(node_->right_ != nullptr) {
                node_ = node_->right_;
            }
        }else {
            // 非header，也无左结点
            base_ptr y = node_->parent_;
            while(node_ == y->left_) {
                node_ = y;
                y = y->parent_;
            }
            node_ = y;  //如果node本身为root，且为最小元素，node就回到了自己
        }
    }

};

template <class T>
struct rb_tree_iterator : public rb_tree_iterator_base<T> {
    typedef T   value_type;
    typedef T&  reference;
    typedef T*  pointer;
    typedef rb_tree_iterator<T> iterator;
    typedef rb_tree_iterator<const T> const_iterator;
    typedef rb_tree_iterator<T> self;

    typedef rb_tree_node_base<T>* base_ptr;
    typedef rb_tree_node<T>*      node_ptr;

    // using rb_tree_iterator_base<T>::node_;

    // 构造函数
    // DEBUG : 这里必须添加this指针或者用using来声明，node_是来自基类的变量，否则编译器不会去基类查找，因为模板在编译器是运行时现场决议的
    // 因此，在编译过程中，对于模板的继承，基类的成员对于派生类是透明的。不过在非模板的情况下，不会出现这种问题，因为编译时就已经决定了。
    rb_tree_iterator() = default;
    rb_tree_iterator(base_ptr x) { this->node_ = x; }
    rb_tree_iterator(node_ptr x) { this->node_ = x; } // 隐式转化
    rb_tree_iterator(const iterator& rhs) { this->node_ = rhs.node_; }
    //rb_tree_iterator(const const_iterator& rhs) { this->node_ = rhs.node_; }

    // 重载操作符
    reference operator*() const { return node_ptr(this->node_)->value_; }
    pointer operator->() const { return &(operator*()); }

    self& operator++() {
        this->increment();
        return *this;
    }

    self operator++(int) {
        self tmp(*this);
        this->increment();
        return tmp;
    }

    self& operator--() {
        this->decrement();
        return *this;
    }

    self operator--(int) {
        self tmp(*this);
        this->decrement();
        return tmp;
    }

    bool operator==(const self& rhs) { return this->node_ == rhs.node_; }
    bool operator!=(const self& rhs) { return this->node_ != rhs.node_; }
};


// tree algo, 一些方便的辅助函数，还有有关树旋转的函数

// return 当前node指向的最小值
template <class NodePtr>
NodePtr rb_tree_min(NodePtr x) {
    while(x->left_ != nullptr) {
        x = x->left_;
    }
    return x;
}

template <class NodePtr>
NodePtr rb_tree_max(NodePtr x) {
    while(x->right_ != nullptr) {
        x = x->right_;
    }
    return x;
}

// 判断当前node是否为其parent的左孩子
template <class NodePtr>
bool rb_tree_is_lchild(NodePtr node) {
    return node == node->parent_->left_;
}

template <class NodePtr>
bool rb_tree_is_rchild(NodePtr node) {
    return node == node->parent_->right_;
}

template <class NodePtr>
bool rb_tree_is_red(NodePtr node) {
    return node->color_ == rb_tree_red;
}

template <class NodePtr>
bool rb_tree_is_black(NodePtr node) {
    return node->color_ == rb_tree_black;
}

template <class NodePtr>
void rb_tree_set_black(NodePtr& node) noexcept
{
  node->color_ = rb_tree_black;
}

template <class NodePtr>
void rb_tree_set_red(NodePtr& node) noexcept
{
  node->color_ = rb_tree_red;
}

template <class NodePtr>
NodePtr rb_tree_next(NodePtr node) {
    if(node->right_ != nullptr) {
        return rb_tree_min(node->right_);
    }
    // 找到第一个作为左孩子的结点
    while(rb_tree_is_rchild(node)) {
        node = node->parent_;
    }
    return node->parent_;
}

// 这里的例子只是举例左旋而已，并不是真正的平衡
/*---------------------------------------*\
|       p                         p       |
|      / \                       / \      |
|     x   d    rotate left      y   d     |
|    / \       ===========>    / \        |
|   a   y                     x   c       |
|      / \                   / \          |
|     b   c                 a   b         |
\*---------------------------------------*/

// 左旋红黑树，参数一为左旋点，参数二为根节点。x为要左旋的子树根节点，root为整个红黑树的root
template <class NodePtr>
void rb_tree_rotate_left(NodePtr x, NodePtr& root) {    // root为指针引用是可能要动root的值
    NodePtr y = x->right_;  // y为x的右子节点
    x->right_ = y->left_;
    if(y->left_ != nullptr) {
        // 如果为空，就不用设置左孩子的parent了
        y->left_->parent_ = x;
    }
    y->parent_ = x->parent_;    // 把y的parent指向p

    // 令y完全顶替x的地位
    if(x == root) {
        // 如果x为根，则让y当root
        root = y;
    }else if(rb_tree_is_lchild(x)) {
        // x为左孩子
        x->parent_->left_ = y;  // 将p的left连接到y，至此p的连接已经完成
    }else {
        x->parent_->right_ = y;
    }

    // 调整x和y的关系
    y->left_ = x;
    x->parent_ = y;
}


/*----------------------------------------*\
|     p                         p          |
|    / \                       / \         |
|   d   x      rotate right   d   y        |
|      / \     ===========>      / \       |
|     y   a                     b   x      |
|    / \                           / \     |
|   b   c                         c   a    |
\*----------------------------------------*/
template <class NodePtr>
void rb_tree_rotate_right(NodePtr x, NodePtr& root) {
    NodePtr y = x->left_;
    x->left_ = y->right_;
    if(y->right_ != nullptr) {
        y->right_->parent_ = x;
    }
    y->parent_ = x->parent_;

    if(x == root) {
        root = y;
    }else if(rb_tree_is_lchild(x)) {
        x->parent_->left_ = y;
    }else {
        x->parent_->right_ = y;
    }

    // 调整x和y
    y->right_ = x;
    x->parent_ = y;
}


// 插入结点后让rb_tree重新平衡，参数一为新增结点，参数二为根节点
//
// 参考博客: http://blog.csdn.net/v_JULY_v/article/details/6105630
//          http://blog.csdn.net/v_JULY_v/article/details/6109153
//
// 插入一共有五种情况，case3 -> case5 其实是一个完整的插入修复的过程，每一种情况的当前结点都为红。
// case1 : 新增节点为根节点。do: 令新增节点为黑（根节点永远为黑）
// case2 : 新增节点的父节点为黑，没有破坏平衡。do: 直接返回
// case3 : 父节点和叔叔节点都为红。do: 令父节点和叔叔节点为黑，祖父节点涂红，让当前结点为祖父结点，转到case4
// case4 : 父节点为红，叔叔节点为nullptr或者黑色，父节点为左(右)孩子，当前结点为右(左)孩子，
//         do: 让父节点成为当前结点，再让当前结点左(右旋)。左右旋取决于当前结点为什么孩子，做出相反的旋转。转到case5
// case5 : 父节点为红，叔叔节点为nullptr或者黑色，父节点为左(右孩子)，当前结点为左(右)孩子，
//         do: 让父节点变为黑色，祖父结点变为红色，以祖父结点为支点右(左)旋。

template <class NodePtr>
void rb_tree_insert_rebalance(NodePtr x, NodePtr& root) {
    rb_tree_set_red(x);     //新增结点一定是红色

    // 如果当前结点为根，或者父节点为黑色，直接结束即可，把root设置为黑
    // 这个循环主要是case3 --> case5 的过程
    while(x != root && rb_tree_is_red(x->parent_)) {
        if(rb_tree_is_lchild(x->parent_)) {
            // 父节点是左子节点
            NodePtr uncle = x->parent_->parent_->right_;
            if(uncle && rb_tree_is_red(uncle)) {
                // case3 : 父节点和叔叔节点都为红，父节点为左孩子
                rb_tree_set_black(x->parent_);  // 父节点和叔叔节点涂黑
                rb_tree_set_black(uncle);
                x = x->parent_->parent_;
                rb_tree_set_red(x);     // 祖父涂红
            }else {
                // 无叔叔节点或者叔叔节点为黑，case4 和 case5，区分两者的条件则是先后顺序，一定会先case4再case5
                if(rb_tree_is_rchild(x)) {
                    // case4 当前结点为右孩子
                    x = x->parent_;
                    rb_tree_rotate_left(x, root);
                }
                // case5, 都转化为了case5,当前结点为左孩子
                rb_tree_set_black(x->parent_);
                rb_tree_set_red(x->parent_->parent_);
                rb_tree_rotate_right(x->parent_->parent_, root);
                
                break;  //结束状态转移
            }
        }
        else {
            // 父节点是右子节点，对称处理
            NodePtr uncle = x->parent_->parent_->left_;
            if(uncle && rb_tree_is_red(uncle)) {
                // case3, 父节点和叔叔节点都为红
                rb_tree_set_black(x->parent_);
                rb_tree_set_black(uncle);
                x = x->parent_->parent_;
                rb_tree_set_red(x);
            }else {
                if(rb_tree_is_lchild(x)) {
                    // case4 : 当前为左孩子
                    x = x->parent_;
                    rb_tree_rotate_right(x, root);
                }
                // case5, 当前结点为右子节点
                rb_tree_set_black(x->parent_);
                rb_tree_set_red(x->parent_->parent_);
                rb_tree_rotate_left(x->parent_->parent_, root);
                break;
            }
        }
    }
    rb_tree_set_black(root);
}

// rb_tree_erase_rebalance
// 删除节点后使 rb tree 重新平衡，参数一为要删除的节点，参数二为根节点，参数三为最小节点，参数四为最大节点
template <class NodePtr>
NodePtr rb_tree_erase_rebalance(NodePtr z, NodePtr& root, NodePtr& leftmost, NodePtr& rightmost) {
    // y 是可能的替换节点，指向最终要删除的节点
  auto y = (z->left_ == nullptr || z->right_ == nullptr) ? z : rb_tree_next(z);
  // x 是 y 的一个独子节点或 NIL 节点
  auto x = y->left_ != nullptr ? y->left_ : y->right_;
  // xp 为 x 的父节点
  NodePtr xp = nullptr;

  // y != z 说明 z 有两个非空子节点，此时 y 指向 z 右子树的最左节点，x 指向 y 的右子节点。
  // 用 y 顶替 z 的位置，用 x 顶替 y 的位置，最后用 y 指向 z
  if (y != z)
  {
    z->left_->parent_ = y;
    y->left_ = z->left_;

    // 如果 y 不是 z 的右子节点，那么 z 的右子节点一定有左孩子
    if (y != z->right_)
    { // x 替换 y 的位置
      xp = y->parent_;
      if (x != nullptr)
        x->parent_ = y->parent_;

      y->parent_->left_ = x;
      y->right_ = z->right_;
      z->right_->parent_ = y;
    }
    else
    {
      xp = y;
    }

    // 连接 y 与 z 的父节点 
    if (root == z)
      root = y;
    else if (rb_tree_is_lchild(z))
      z->parent_->left_ = y;
    else
      z->parent_->right_ = y;
    y->parent_ = z->parent_;
    mystl::swap(y->color_, z->color_);
    y = z;
  }
  // y == z 说明 z 至多只有一个孩子
  else
  { 
    xp = y->parent_;
    if (x)  
      x->parent_ = y->parent_;

    // 连接 x 与 z 的父节点
    if (root == z)
      root = x;
    else if (rb_tree_is_lchild(z))
      z->parent_->left_ = x;
    else
      z->parent_->right_ = x;

    // 此时 z 有可能是最左节点或最右节点，更新数据
    if (leftmost == z)
      leftmost = x == nullptr ? xp : rb_tree_min(x);
    if (rightmost == z)
      rightmost = x == nullptr ? xp : rb_tree_max(x);
  }

  // 此时，y 指向要删除的节点，x 为替代节点，从 x 节点开始调整。
  // 如果删除的节点为红色，树的性质没有被破坏，否则按照以下情况调整（x 为左子节点为例）：
  // case 1: 兄弟节点为红色，令父节点为红，兄弟节点为黑，进行左（右）旋，继续处理
  // case 2: 兄弟节点为黑色，且两个子节点都为黑色或 NIL，令兄弟节点为红，父节点成为当前节点，继续处理
  // case 3: 兄弟节点为黑色，左子节点为红色或 NIL，右子节点为黑色或 NIL，
  //         令兄弟节点为红，兄弟节点的左子节点为黑，以兄弟节点为支点右（左）旋，继续处理
  // case 4: 兄弟节点为黑色，右子节点为红色，令兄弟节点为父节点的颜色，父节点为黑色，兄弟节点的右子节点
  //         为黑色，以父节点为支点左（右）旋，树的性质调整完成，算法结束
  if (!rb_tree_is_red(y))
  { // x 为黑色时，调整，否则直接将 x 变为黑色即可
    while (x != root && (x == nullptr || !rb_tree_is_red(x)))
    {
      if (x == xp->left_)
      { // 如果 x 为左子节点
        auto brother = xp->right_;
        if (rb_tree_is_red(brother))
        { // case 1
          rb_tree_set_black(brother);
          rb_tree_set_red(xp);
          rb_tree_rotate_left(xp, root);
          brother = xp->right_;
        }
        // case 1 转为为了 case 2、3、4 中的一种
        if ((brother->left_ == nullptr || !rb_tree_is_red(brother->left_)) &&
            (brother->right_ == nullptr || !rb_tree_is_red(brother->right_)))
        { // case 2
          rb_tree_set_red(brother);
          x = xp;
          xp = xp->parent_;
        }
        else
        { 
          if (brother->right_ == nullptr || !rb_tree_is_red(brother->right_))
          { // case 3
            if (brother->left_ != nullptr)
              rb_tree_set_black(brother->left_);
            rb_tree_set_red(brother);
            rb_tree_rotate_right(brother, root);
            brother = xp->right_;
          }
          // 转为 case 4
          brother->color_ = xp->color_;
          rb_tree_set_black(xp);
          if (brother->right_ != nullptr)  
            rb_tree_set_black(brother->right_);
          rb_tree_rotate_left(xp, root);
          break;
        }
      }
      else  // x 为右子节点，对称处理
      { 
        auto brother = xp->left_;
        if (rb_tree_is_red(brother))
        { // case 1
          rb_tree_set_black(brother);
          rb_tree_set_red(xp);
          rb_tree_rotate_right(xp, root);
          brother = xp->left_;
        }
        if ((brother->left_ == nullptr || !rb_tree_is_red(brother->left_)) &&
            (brother->right_ == nullptr || !rb_tree_is_red(brother->right_)))
        { // case 2
          rb_tree_set_red(brother);
          x = xp;
          xp = xp->parent_;
        }
        else
        {
          if (brother->left_ == nullptr || !rb_tree_is_red(brother->left_))
          { // case 3
            if (brother->right_ != nullptr)
              rb_tree_set_black(brother->right_);
            rb_tree_set_red(brother);
            rb_tree_rotate_left(brother, root);
            brother = xp->left_;
          }
          // 转为 case 4
          brother->color_ = xp->color_;
          rb_tree_set_black(xp);
          if (brother->left_ != nullptr)  
            rb_tree_set_black(brother->left_);
          rb_tree_rotate_right(xp, root);
          break;
        }
      }
    }
    if (x != nullptr)
      rb_tree_set_black(x);
  }
  return y;
}


// 红黑树Compare为键值比较函数，默认为less，最好自己传入比较key方法的函数
template <class Key, class T, class Compare = mystl::less<Key>, class KeyofValue = mystl::identity<T>>
class rb_tree {
public:
    // typedef
    typedef rb_tree_node_base<T>  base_type;
    typedef rb_tree_node_base<T>* base_ptr;
    typedef rb_tree_node<T>*      node_ptr;
    typedef rb_tree_node<T>       node_type;
    
    typedef T                   value_type;
    typedef Key                 key_type;       // 将返回值类型定义为key       
    typedef Compare             key_compare;    
    
    typedef mystl::allocator<T>         allocator_type;
    typedef mystl::allocator<T>         data_allocator;
    typedef mystl::allocator<base_type> base_allocator;
    typedef mystl::allocator<node_type> node_allocator;

    typedef T*          pointer;
    typedef const T*    const_pointer;
    typedef T&          reference;
    typedef const T&    const_reference;
    typedef size_t      size_type;
    typedef ptrdiff_t   difference_type;

    typedef rb_tree_iterator<T>                     iterator;
    typedef rb_tree_iterator</* const  */T>               const_iterator;
    typedef mystl::reverse_iterator<iterator>       reverse_iterator;
    typedef mystl::reverse_iterator<const_iterator> const_reverse_iterator;

    allocator_type get_allocator() const { return node_allocator(); }
    key_compare    key_comp()      const { return key_comp_; }

private:
    // 只用3笔数据表示一个rb_tree
    base_ptr    header_;        // header结点，和根节点互为父节点，且为红色
    size_type   node_count_;    // 红黑树结点的数量
    key_compare key_comp_;      // 节点元素键值的比较规则

private:
    // 以下三个函数返回root，最小值结点和最大值结点的引用
    base_ptr& root()        const { return header_->parent_; }
    base_ptr& leftmost()    const { return header_->left_; }
    base_ptr& rightmost()   const { return header_->right_; }

public:
    // big-five
    rb_tree() { rb_tree_init(); }
    rb_tree(const rb_tree& rhs);
    rb_tree(rb_tree&& rhs );

    rb_tree& operator=(const rb_tree& rhs);
    rb_tree& operator=(rb_tree&& rhs);

    ~rb_tree() { clear(); }

public:
    iterator begin() { return leftmost(); }     // 隐式转化
    const_iterator begin() const { return leftmost(); }
    iterator end() { return header_; }
    const_iterator end() const { return header_; }

    reverse_iterator rbegin() { return reverse_iterator(end()); }
    const_reverse_iterator rbegin() const { return const_reverse_iterator(end()); }
    reverse_iterator rend() { return reverse_iterator(begin()); }
    const_reverse_iterator rend() const { return const_reverse_iterator(begin()); }

    const_iterator cbegin() const { return begin(); }
    const_iterator cend() const { return end(); }
    const_reverse_iterator crbegin() const { return rbegin(); }
    const_reverse_iterator crend() const { return rend(); }

    // 容量相关
    bool empty() const { return node_count_ == 0; }
    size_type size() const { return node_count_; }
    size_type max_size() const { return static_cast<size_type>(-1); }

    // insert , 两种插入，unique不允许重复，equal允许重复。重复元素相对顺序稳定

    mystl::pair<iterator, bool> insert_unique(const value_type& value);
    iterator insert_equal(const value_type& value);

    // 本质来说，rb_tree是不提供pos的，有可能pos位置出错，但是这里检查pos是否正确，如果正确，调用insert_aux，少了logn复杂度的查找
    // 如果不正确，就会调用相对于的insert_xxx(value)版本，从根节点查找，找到正确的结点位置插入
    iterator insert_unique(iterator pos, const value_type& value);
    iterator insert_equal(iterator pos, const value_type& value);

    template <class Iterator>
    void insert_unique(Iterator first, Iterator last) {
        for(; first != last ; ++first) {
            insert_unique(*first);
        }
    }

    template <class Iterator>
    void insert_equal(Iterator first, Iterator last) {
        for(; first != last ; ++first) {
            insert_equal(*first);
        }
    }


    // erase
    iterator erase(iterator pos);
    size_type erase(const key_type& key);   //返回删除的数量
    void erase(iterator first, iterator last);


    // clear
    void clear();

    // rb_tree 独有的操作

    // find 如果有key重复，返回首先入红黑树的，也就是第一个重复元素
    iterator find (const key_type& key);
    const_iterator find(const key_type& key) const;

    size_type count(const key_type& key) const; // 返回key的元素的数量

    // 二分查找，同全局的算法库结果一样
    iterator lower_bound(const key_type& key);
    const_iterator lower_bound(const key_type& key) const;

    iterator upper_bound(const key_type& key);
    const_iterator upper_bound(const key_type& key) const;

    // 返回key值的范围，[first, last)区间
    mystl::pair<iterator,iterator> equal_range(const key_type& x) {
        return mystl::pair<iterator,iterator>(lower_bound(x), upper_bound(x));
    }
    mystl::pair<const_iterator, const_iterator> equal_range(const key_type& x) const {
        return mystl::pair<const_iterator, const_iterator>(lower_bound(x), upper_bound(x));
    }

    // swap
    void swap(rb_tree& rhs) {
        if(this != &rhs) {
            mystl::swap(header_, rhs.header_);
            mystl::swap(node_count_, rhs.node_count_);
            mystl::swap(key_comp_, rhs.key_comp_);
        }
    }

private:
    // helper func

    // 初始化
    void rb_tree_init();    
    void reset() {
        header_ = nullptr;  // 把指针置空而已
        node_count_ = 0;
    }

    // 与结点内存相关
    node_ptr create_node(const value_type& v) {
        node_ptr tmp = node_allocator::allocate(1); // 分配一个结点的内存
        try{
            mystl::construct(&tmp->value_, v);
        }catch(...) {
            node_allocator::deallocate(tmp);
            throw;
        }
        return tmp;
    }

    // 仅仅clone颜色和value
    node_ptr clone_node(node_ptr p) {
        node_ptr node = create_node(p->value_);
        node->color_ = p->color_;
        node->left_ = nullptr;
        node->right_ = nullptr;
        node->parent_ = nullptr;
        return node;
    }

    void destroy_node(node_ptr node) {
        mystl::destroy(&node->value_);
        node_allocator::deallocate(node);
    }

    // 在x出插入value，y为x的父节点
    iterator insert_aux(base_ptr x, base_ptr y, const value_type& value);

    // copy / erase tree

    node_ptr copy_from(node_ptr x, node_ptr p);
    node_ptr copy_from1(node_ptr x, node_ptr p);

    // 以x为根，递归删除结点
    void erase_since(base_ptr x);
};

// header初始情况，结点为红，parent == nullptr, left和right指向自己
template <class Key, class T, class Compare, class KeyofValue>
void rb_tree<Key, T, Compare, KeyofValue>::rb_tree_init() {
    header_ = node_allocator::allocate(1);
    header_->color_ = rb_tree_red;
    root() = nullptr;
    leftmost() = header_;
    rightmost() = header_;
    node_count_ = 0;
}

template <class Key, class T, class Compare, class KeyofValue>
rb_tree<Key, T, Compare, KeyofValue>::rb_tree(const rb_tree& rhs) {
    rb_tree_init();     //初始化header
    if(rhs.node_count_ != 0) {
        root() = copy_from(reinterpret_cast<node_ptr>(rhs.root()), reinterpret_cast<node_ptr>(header_));  // 由于这个函数传入的指针必须非空，所以要判定
        leftmost() = rb_tree_min(root());
        rightmost() = rb_tree_max(root());
    }
    node_count_ = rhs.node_count_;
    key_comp_ = rhs.key_comp_;
}

template <class Key, class T, class Compare, class KeyofValue>
rb_tree<Key, T, Compare, KeyofValue>::rb_tree(rb_tree&& rhs) : 
        header_(mystl::move(rhs.header_)), 
        node_count_(rhs.node_count_),
        key_comp_(rhs.key_comp_) {
    rhs.reset();
}

template <class Key, class T, class Compare, class KeyofValue>
rb_tree<Key, T, Compare, KeyofValue>& 
rb_tree<Key, T, Compare, KeyofValue>::operator=(const rb_tree& rhs) {
    if(this != &rhs) {
        clear();    //首先清空本身

        if(rhs.node_count_ != 0) {
            root() = copy_from(reinterpret_cast<node_ptr>(rhs.root()), reinterpret_cast<node_ptr>(header_));
            leftmost() = rb_tree_min(root());
            rightmost() = rb_tree_max(root());
        }
        node_count_ = rhs.node_count_;
        key_comp_ = rhs.key_comp_;
    }
    return *this;
}

template <class Key, class T, class Compare, class KeyofValue>
rb_tree<Key, T, Compare, KeyofValue>& 
rb_tree<Key, T, Compare, KeyofValue>::operator=(rb_tree&& rhs) {
    clear();
    header_ = mystl::move(rhs.header_);
    node_count_ = rhs.node_count_;
    key_comp_ = rhs.key_comp_;
    rhs.reset();        //指针置空
    return *this;
}


// 当y == header满足，那么size == 0,否则不可能y == header
// 当插入到一个node的左边时，x == y，直接进第一个分支
template <class Key, class T, class Compare, class KeyofValue>
typename rb_tree<Key, T, Compare, KeyofValue>::iterator 
rb_tree<Key, T, Compare, KeyofValue>::insert_aux(base_ptr x_, base_ptr y_, const value_type& value) {
    node_ptr x = reinterpret_cast<node_ptr>(x_);
    node_ptr y = reinterpret_cast<node_ptr>(y_);
    node_ptr z;  // 表示新增结点
    // x == nullptr 是一定的
    if(y == header_ || x || key_comp_(KeyofValue()(value), KeyofValue()(y->value_))) {
        // 3种情况
        z = create_node(value);
        // x 为 y的左孩子
        y->left_ = z;
        if(header_ == y){
            root() = z;
            rightmost() = z;    // 更新最大值
        }else if(y == leftmost()) {
            // y为最左结点
            leftmost() = z;
        }
    }else {
        // x为y的右孩子
        z = create_node(value);
        y->right_ = z;
        if(y == rightmost()) {
            rightmost() = z;
        }
    }

    // 设置z的指针
    z->parent_ = y;
    z->left_ = nullptr;
    z->right_ = nullptr;
    // x新节点的设置color == red, 是在rebalance函数中的

    rb_tree_insert_rebalance(static_cast<base_ptr>(z), header_->parent_);
    ++node_count_;
    return iterator(z);
}


template <class Key, class T, class Compare, class KeyofValue>
mystl::pair<typename rb_tree<Key, T, Compare, KeyofValue>::iterator, bool> 
rb_tree<Key, T, Compare, KeyofValue>::insert_unique(const value_type& value) {
    base_ptr y = header_;
    base_ptr x = root();
    bool comp = true;
    while(x) {
        y = x;
        comp = key_comp_(KeyofValue()(value), KeyofValue()(x->get_node_ptr()->value_));
        x = comp ? x->left_ : x->right_;    // true表示最后一次往左边走
    }
    iterator j = iterator(y);   // 把j作为插入结点的父节点

    if(comp) {
        if(j == begin()) {
            // 插入结点的父节点是最左结点
            return mystl::pair<iterator, bool>(insert_aux(x, y, value), true);  // 一定可以插入成功
        }else {
            --j;    // j倒退一个，因为不是第一个结点的左边，而是中间结点的左边，所以--，找到*j <= value,为了后面判断重复元素.  此时是j自减，y并没有动
        }
    }
    if(key_comp_(KeyofValue()(*j), KeyofValue()(value))) {
        return  mystl::pair<iterator, bool>(insert_aux(x, y, value), true);  // *j < value 
    }

    // 第一个if(false)， value >= *j. 第二个if(false),  value <= *j。
    // 则走到这个分支，一定有， value == *j。结点重复，返回重复值的结点
    return mystl::pair<iterator, bool>(j, false);
}



// 允许插入的值key重复，且稳定，因为相等的情况，会走向右边
template <class Key, class T, class Compare, class KeyofValue>
typename rb_tree<Key, T, Compare, KeyofValue>::iterator
rb_tree<Key, T, Compare, KeyofValue>::insert_equal(const value_type& value) {
    base_ptr y = header_;
    base_ptr x = root();     //y为x的父亲
    while(x) {
        y = x;
        x = key_comp_(KeyofValue()(value), KeyofValue()(x->get_node_ptr()->value_)) 
                ? x->left_ : x->right_; // 比较value与x->value大小，小则去左边
    }
    return insert_aux(x, y, value);
}


// 这样分类讨论，是为了让查找次数减少，如果pos正确，直接调用insert_aux，是O(1)的复杂度
template <class Key, class T, class Compare, class KeyofValue>
typename rb_tree<Key, T, Compare, KeyofValue>::iterator 
rb_tree<Key, T, Compare, KeyofValue>::insert_unique(iterator pos, const value_type& value) {
    if(pos.node_ == header_->left_) {   // begin()
        if(size() > 0 && key_comp_(KeyofValue()(value), KeyofValue()(*pos))) {
            // 保证有根节点，插入到最左侧结点的左侧，size > 0保证比较合法
            return insert_aux(pos.node_, pos.node_, value); //保证first arg非空即可，这样就走到了结点左侧插入
        }else {
            // 直接调用insert_unique(value)形式
            return insert_unique(value).first;
        }
    }else if(pos.node_ == header_) {    // end()
        if(/* size() && */ key_comp_(KeyofValue()(rightmost()->get_node_ptr()->value_), KeyofValue()(value))) {
            // 不用保证size() > 0，如果size == 0，那么pos == begin()成立，此时begin() == end()，优先匹配第一种情况
            return insert_aux(nullptr, rightmost(), value);
        }else {
            return insert_unique(value).first;
        }
    }else {
        // 这里size() > 2 恒成立，否则必定会走上面两个分支
        iterator before = pos;
        --before;       // 获得前一个迭代器
        if(key_comp_(KeyofValue()(*before), KeyofValue()(value)) 
            && key_comp_(KeyofValue()(value), KeyofValue()(*pos))) {   // before < value < pos
            // 说明pos的指示是正确的，按照情况，插入到before，后者pos的地方
            if(before.node_->right_ == nullptr) {
                // before无右孩子，直接插入到before的右侧
                return insert_aux(nullptr, before.node_, value);
            }else {
                // before如果有右孩子，那么pos一定没有左孩子，因为两个迭代器是连续的，所以要么插入到before的右边，要么插入到pos的左边
                return insert_aux(pos.node_, pos.node_, value);
            }
        }else {
            return insert_unique(value).first;  // 说明pos指示错了
        }
    }
}

template <class Key, class T, class Compare, class KeyofValue>
typename rb_tree<Key, T, Compare, KeyofValue>::iterator 
rb_tree<Key, T, Compare, KeyofValue>::insert_equal(iterator pos, const value_type& value) {
    if(pos.node_ == header_->left_) {
        if(size() > 0 && key_comp_(KeyofValue()(value), KeyofValue()(*pos))) {   // v < *pos, v == *pos ：调用equal，否则不稳定
            return insert_aux(pos.node_, pos.node_, value);   //相等的情况，不在里面，否则不能保持稳定
        }else {
            return insert_equal(value);
        }
    }else if(pos.node_ == header_) {    // end()
        if(!(key_comp_(KeyofValue()(value), KeyofValue()(rightmost()->get_node_ptr()->value_)))) {  // *pos <= value ，插入到右边
            return insert_aux(nullptr, rightmost(), value);
        }else {
            // value > *pos 不成立
            return insert_equal(value);
        }
    }else {
        iterator before = pos;
        --before;
        if( !(key_comp_(KeyofValue()(value), KeyofValue()(*before))) 
            && key_comp_(KeyofValue()(value), KeyofValue()(*pos))) {
           // *before <= value < *pos，才会插入到这样的区间内。如果右侧不等式加上等号，那就是不稳定的 
           if(before.node_->right_ == nullptr) {
                return  insert_aux(nullptr, before.node_, value);
           }else {
                return  insert_aux(pos.node_, pos.node_, value);
           }
        }else {
            return insert_equal(value);
        }
    }
}



// 递归复制一棵树，被复制的树当前结点为x，当前树的当前结点的父节点为p。 不是x的parent
// 这里所有的右节点采用递归复制，而左节点采用循环复制
// 之所以要传入p，是因为红黑树还要设置parent，如果是单纯的二叉树，不用这么麻烦
template <class Key, class T, class Compare, class KeyofValue>
typename rb_tree<Key, T, Compare, KeyofValue>::node_ptr 
rb_tree<Key, T, Compare, KeyofValue>::copy_from(node_ptr x, node_ptr p) {
    node_ptr top = clone_node(x);
    top->parent_ = p;   //设置当前的parent

    // x右子树非空
    if(x->right_) {
        top->right_ = copy_from(reinterpret_cast<node_ptr>(x->right_), reinterpret_cast<node_ptr>(top));
    }

    p = top;    // parent下沉
    x = reinterpret_cast<node_ptr>(x->left_);   // 轮到复制左子树了
    while(x != nullptr) {
        node_ptr y = clone_node(x);
        p->left_ = y;
        y->parent_ = p;
        if(x->right_) {
            y->right_ = copy_from(reinterpret_cast<node_ptr>(x->right_), reinterpret_cast<node_ptr>(y));    // 右节点递归，左结点循环
        }
        p = y;
        x = reinterpret_cast<node_ptr>(x->left_);
    }
    return top;
}

// 左右子树届递归处理
template <class Key, class T, class Compare, class KeyofValue>
typename rb_tree<Key, T, Compare, KeyofValue>::node_ptr 
rb_tree<Key, T, Compare, KeyofValue>::copy_from1(node_ptr x, node_ptr p) {
    node_ptr top = clone_node(x);
    top->parent_ = p;   //设置当前的parent

    if(x->right_) {
        top->right_ = copy_from1(reinterpret_cast<node_ptr>(x->right_), top);
    }
    if(x->left_) {
        top->left_ = copy_from1(reinterpret_cast<node_ptr>(x->left_), top);
    }
    return top;
}


// find 如果有key重复，返回首先入红黑树的，也就是第一个重复元素
template <class Key, class T, class Compare, class KeyofValue>
typename rb_tree<Key, T, Compare, KeyofValue>::iterator 
rb_tree<Key, T, Compare, KeyofValue>::find (const key_type& key) {
    base_ptr y = header_;
    base_ptr x = root();

    while(x != nullptr) {
        if(!key_comp_(KeyofValue()(x->get_node_ptr()->value_), key) ) {
            // x >= k ，往左走， == 也是往左走，找到第一个
            y = x;
            x = x->left_;
        }else{
            x = x->right_;  // x < k 往右走
        }
    }
    iterator j = iterator(y);   // j应该为找到的结点

    // 没找到有两种情况，就是j==end()，说明元素为空。另一种情况就是，没有这个元素，j指向本该插入的位置，k < *j
    return (j == end() || key_comp_(key, KeyofValue()(*j))) ? end() : j;
}

template <class Key, class T, class Compare, class KeyofValue>
typename rb_tree<Key, T, Compare, KeyofValue>::const_iterator 
rb_tree<Key, T, Compare, KeyofValue>::find(const key_type& key) const {
    base_ptr y = header_;
    base_ptr x = root();

    while(x != nullptr) {
        if(!key_comp_(KeyofValue()(x->get_node_ptr()->value_), key) ) {
            // x >= k ，往左走， == 也是往左走，找到第一个
            // 如果往左走，说明x为一个>=key的值，所以用y记录住
            y = x;
            x = x->left_;
        }else{
            x = x->right_;  // x < k 往右走
        }
    }
    const_iterator j = const_iterator(y);   // j应该为找到的结点

    return (j == end() || key_comp_(key, KeyofValue()(*j))) ? end() : j;
}


// lower_bound算法和find一样
template <class Key, class T, class Compare, class KeyofValue>
typename rb_tree<Key, T, Compare, KeyofValue>::iterator 
rb_tree<Key, T, Compare, KeyofValue>::lower_bound(const key_type& key) {
    base_ptr y = header_;
    base_ptr x = root();

    while(x != nullptr) {
        if(!key_comp_(KeyofValue()(x->get_node_ptr()->value_), key) ) {
            // x >= k ，往左走， == 也是往左走，找到第一个
            y = x;
            x = x->left_;
        }else{
            x = x->right_;  // x < k 往右走
        }
    }
    
    return iterator(y);     // 没找到正好返回该插入的位置
}

template <class Key, class T, class Compare, class KeyofValue>
typename rb_tree<Key, T, Compare, KeyofValue>::const_iterator 
rb_tree<Key, T, Compare, KeyofValue>::lower_bound(const key_type& key) const {
    base_ptr y = header_;
    base_ptr x = root();

    while(x != nullptr) {
        if(!key_comp_(KeyofValue()(x->get_node_ptr()->value_), key) ) {
            // x >= k ，往左走， == 也是往左走，找到第一个
            y = x;
            x = x->left_;
        }else{
            x = x->right_;  // x < k 往右走
        }
    }
    
    return const_iterator(y);
}

// upper_bound在比较的时候把==的情况，往右走即可
template <class Key, class T, class Compare, class KeyofValue>
typename rb_tree<Key, T, Compare, KeyofValue>::iterator 
rb_tree<Key, T, Compare, KeyofValue>::upper_bound(const key_type& key) {
    base_ptr y = header_;
    base_ptr x = root();

    while(x != nullptr) {
        if(key_comp_(key, KeyofValue()(x->get_node_ptr()->value_)) ) {
            // k < x ，往左走， 
            y = x;
            x = x->left_;
        }else{
            x = x->right_;  // x < k 往右走, == 也是往右走，找到右侧开区间
        }
    }
    
    return iterator(y);     // 没找到正好返回该插入的位置
}

template <class Key, class T, class Compare, class KeyofValue>
typename rb_tree<Key, T, Compare, KeyofValue>::const_iterator 
rb_tree<Key, T, Compare, KeyofValue>::upper_bound(const key_type& key) const {
    base_ptr y = header_;
    base_ptr x = root();

    while(x != nullptr) {
        if(key_comp_(key, KeyofValue()(x->get_node_ptr()->value_)) ) {
            // k < x ，往左走， 
            y = x;
            x = x->left_;
        }else{
            x = x->right_;  // x < k 往右走, == 也是往右走，找到右侧开区间
        }
    }
    
    return const_iterator(y);     // 没找到正好返回该插入的位置
}

template <class Key, class T, class Compare, class KeyofValue>
typename rb_tree<Key, T, Compare, KeyofValue>::size_type 
rb_tree<Key, T, Compare, KeyofValue>::count(const key_type& key) const {
    auto p = equal_range(key);
    return mystl::distance(p.first, p.second);
}

template <class Key, class T, class Compare, class KeyofValue>
typename rb_tree<Key, T, Compare, KeyofValue>::iterator 
rb_tree<Key, T, Compare, KeyofValue>::erase(iterator pos) {
    iterator next(pos);
    ++next;
    
    base_ptr y = rb_tree_erase_rebalance(pos.node_, root(), leftmost(), rightmost());
    destroy_node(y->get_node_ptr());    // 销毁该node
    --node_count_;
    return next;
}


template <class Key, class T, class Compare, class KeyofValue>
typename rb_tree<Key, T, Compare, KeyofValue>::size_type  
rb_tree<Key, T, Compare, KeyofValue>::erase(const key_type& key) {
    auto p = equal_range(key);
    size_type n = mystl::distance(p.first, p.second);
    erase(p.first, p.second);
    return n;
}

template <class Key, class T, class Compare, class KeyofValue>
void  rb_tree<Key, T, Compare, KeyofValue>::erase(iterator first, iterator last) {
    if(first == begin() && last == end()) {
        clear();
    }else {
        while(first != last) {
            erase(first++);
        }
    }  
}

template <class Key, class T, class Compare, class KeyofValue>
void rb_tree<Key, T, Compare, KeyofValue>::clear() {
    // 仅剩下header
    if(node_count_ != 0) {
        erase_since(root());
        leftmost() = header_;
        rightmost() = header_;
        root() = nullptr;
        node_count_ = 0;
    }
}

template <class Key, class T, class Compare, class KeyofValue>
void rb_tree<Key, T, Compare, KeyofValue>::erase_since(base_ptr x) {
    while(x != nullptr) {
        erase_since(x->right_); // 删除右子树

        base_ptr y = x->left_;
        destroy_node(x->get_node_ptr());    // 删除当前结点
        x = y;  // 循环删除左子树
    }
}


// 重载比较操作符
template <class T, class Compare>
bool operator==(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
{
  return lhs.size() == rhs.size() && mystl::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <class T, class Compare>
bool operator<(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
{
  return mystl::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <class T, class Compare>
bool operator!=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
{
  return !(lhs == rhs);
}

template <class T, class Compare>
bool operator>(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
{
  return rhs < lhs;
}

template <class T, class Compare>
bool operator<=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
{
  return !(rhs < lhs);
}

template <class T, class Compare>
bool operator>=(const rb_tree<T, Compare>& lhs, const rb_tree<T, Compare>& rhs)
{
  return !(lhs < rhs);
}

// 重载 mystl 的 swap
template <class T, class Compare>
void swap(rb_tree<T, Compare>& lhs, rb_tree<T, Compare>& rhs) noexcept
{
  lhs.swap(rhs);
}

}

#endif
