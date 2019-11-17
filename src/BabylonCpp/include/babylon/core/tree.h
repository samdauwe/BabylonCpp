#ifndef BABYLON_CORE_TREE_H
#define BABYLON_CORE_TREE_H

#include <functional>
#include <iostream>
#include <memory>
#include <vector>

namespace BABYLON {

/**
 * @brief Represents a node of the Tree<T> class. The TreeNode<T> is also a
 * container, and can be thought of as instrumentation to determine the location
 * of the type T in the Tree<T>.
 */
template <typename T>
class TreeNode {

public:
  using node_iterator       = typename std::vector<std::unique_ptr<TreeNode<T>>>::iterator;
  using const_node_iterator = typename std::vector<std::unique_ptr<TreeNode<T>>>::const_iterator;
  using size_type           = typename std::vector<std::unique_ptr<TreeNode<T>>>::size_type;

public:
  /**
   * @brief Convenience constructor to create a Node<T> with an instance of T.
   * @param data an instance of T.
   */
  TreeNode(T data) : _id{uid++}, _data{std::move(data)}, _parent{nullptr}
  {
  }

  /**
   * @brief Destructor
   */
  ~TreeNode() = default;

  /**
   * @brief Returns the unique node identifier.
   * @return The unique node identifier.
   */
  [[nodiscard]] size_type id() const
  {
    return _id;
  }

  /**
   * @brief Returns the data object stored in the node.
   * @return The data object stored in the node.
   */
  T& data()
  {
    return _data;
  }

  /**
   * @brief Returns the data object stored in the node.
   * @return The data object stored in the node.
   */
  [[nodiscard]] const T& data() const
  {
    return _data;
  }

  /**
   * @brief Sets a new data object.
   * @param newData The new data object to store.
   */
  void setData(const T& newData)
  {
    _data = newData;
  }

  /**
   * @brief Returns whether or not this node is the root node.
   * @return true if this node is the root node, false otherwise.
   */
  [[nodiscard]] bool isRoot() const
  {
    return _parent == nullptr;
  }

  /**
   * @brief Returns the number of child nodes.
   * @return The number of child nodes.
   */
  [[nodiscard]] size_type arity() const
  {
    return _children.size();
  }

  /**
   * @brief Returns whether or not this node is a leaf node.
   * @return true if this node is a leaf node, false otherwise.
   */
  [[nodiscard]] bool isLeaf() const
  {
    return _children.empty();
  }

  /**
   * @brief Returns a pointer to the parent node.
   * @return A pointer to the parent node.
   */
  TreeNode<T>*& parent()
  {
    return _parent;
  }

  /**
   * @brief Returns a pointer to the parent node.
   * @return A pointer to the parent node.
   */
  [[nodiscard]] const TreeNode<T>* parent() const
  {
    return _parent;
  }

  // Iterator access to child nodes

  node_iterator child_begin()
  {
    return _children.begin();
  }

  [[nodiscard]] const_node_iterator child_begin() const
  {
    return _children.begin();
  }

  node_iterator child_end()
  {
    return _children.end();
  }

  [[nodiscard]] const_node_iterator child_end() const
  {
    return _children.end();
  }

  /**
   * @brief Returns a pointer to the first child.
   * @return A pointer to the first child.
   */
  [[nodiscard]] TreeNode<T>* firstChild() const
  {
    return _children.front().get();
  }

  /**
   * @brief Returns a pointer to the last child.
   * @return A pointer to the last child.
   */
  [[nodiscard]] TreeNode<T>* lastChild() const
  {
    return _children.back().get();
  }

  void addChildren(const std::vector<T>& dataItems)
  {
    for (auto& dataItem : dataItems) {
      addChild(dataItem);
    }
  }

  /**
   * @brief Creates a child node using the given data object.
   * @param data The data object to be encapsulated in the new child node.
   * @return Reference to the created node.
   */
  TreeNode<T>& addChild(T data)
  {
    auto node     = std::make_unique<TreeNode<T>>(data);
    node->_parent = this;
    _children.emplace_back(std::move(node));
    return *_children.back();
  }

  /**
   * @brief Creates a child node using the given data object and adds the item
   * defined by the comparator argument.
   * @param data The data object to be encapsulated in the new child node.
   * @param comparator The comparator function to insert a new element in a
   * sorted vector.
   * @return Reference to the created node.
   */
  TreeNode<T>& addChildSorted(T data, const std::function<bool(const T& a, const T& b)>& comparator)
  {
    auto node     = std::make_unique<TreeNode<T>>(data);
    node->_parent = this;
    auto pos      = std::find_if(_children.begin(), _children.end(),
                            [&node, &comparator](const std::unique_ptr<TreeNode<T>>& treeNode) {
                              return comparator(node->data(), treeNode->data());
                            });
    auto it       = _children.insert(pos, std::move(node));
    return *(*it);
  }

  /**
   * @brief Returns the node depth. A node depth of zero means that the node is
   * the root node of the tree.
   * @return The node level.
   */
  [[nodiscard]] unsigned int depth() const
  {
    if (isRoot()) {
      return 0;
    }
    else {
      return _parent->depth() + 1;
    }
  }

  /**
   * @brief Returns the list containing the children nodes.
   * @return The list containing the children nodes.
   */
  std::vector<std::unique_ptr<TreeNode<T>>>& children()
  {
    return _children;
  }

private:
  static std::size_t uid;
  std::size_t _id;
  T _data;
  TreeNode* _parent;
  std::vector<std::unique_ptr<TreeNode<T>>> _children;

}; // end of class TreeNode

/**
 * @brief Represents a Tree of Objects of generic type T. The Tree is
 * represented as a single root node which points to a
 * std::vector<TreeNode<T>>
 * of children. There is no restriction on the number of children that a
 * particular node may have. This Tree provides a method to serialize the Tree
 * into a List by doing a pre-order traversal. It has several methods to allow
 * easy updation of nodes in the tree.
 */
template <typename T>
class Tree {

private:
  std::unique_ptr<TreeNode<T>> _root;

public:
  class iterator;
  using const_iterator = iterator;
  using value_type     = T;

  /**
   * @brief Default constructor.
   */
  Tree() : _root{std::make_unique<TreeNode<T>>()}
  {
  }

  /**
   * @brief constructor.
   */
  Tree(T data) : _root{std::make_unique<TreeNode<T>>(std::move(data))}
  {
  }

  /**
   * @brief Destructor.
   */
  ~Tree()
  {
    releaseSubtree(_root);
    _root.reset(nullptr);
  }

  /**
   * @brief Returns a reference to the root node.
   * @return A reference to the root node.
   */
  TreeNode<T>& root()
  {
    return *_root;
  }

  /**
   * @brief Returns a const reference to the root node.
   * @return A const reference to the root node.
   */
  [[nodiscard]] const TreeNode<T>& root() const
  {
    return *_root;
  }

  /**
   * @brief Returns the root pointer.
   * @return The root pointer.
   */
  std::unique_ptr<TreeNode<T>>& rootPtr()
  {
    return _root;
  }

  /**
   * @brief Returns the root const pointer.
   * @return The root const pointer.
   */
  [[nodiscard]] const std::unique_ptr<TreeNode<T>>& rootPtr() const
  {
    return _root;
  }

  /**
   * @brief Recursively visits each node of the tree and calls the visitor
   * lambda
   * @param node the node to visit
   * @param visitor callback lambda
   */
  static void recursive_visit(TreeNode<T>& node,
                              const std::function<void(TreeNode<T>& node)>& visitor)
  {
    if (node.isLeaf()) {
      visitor(node);
    }
    else {
      visitor(node);
      for (const auto& child : node.children()) {
        recursive_visit(*child, visitor);
      }
    }
  }

  /**
   * @brief Returns an interator to the top level node.
   * @return An interator to the root node.
   */
  [[nodiscard]] iterator begin() const
  {
    const auto& rootOfTree = _root.get();
    return iterator(rootOfTree, rootOfTree);
  }

  /**
   * @brief Returns an interator to the end of the tree.
   * @return An interator to end of the tree (= nullptr node).
   */
  [[nodiscard]] iterator end() const
  {
    const auto& rootOfTree = _root.get();
    return iterator(nullptr, rootOfTree);
  }

  /**
   * @brief Returns a const interator to the top level node.
   * @return A const interator to the root node.
   */
  [[nodiscard]] const_iterator cbegin() const
  {
    return begin();
  }

  /**
   * @brief Returns a const interator to the end of the tree.
   * @return A const interator to end of the tree (= nullptr node).
   */
  [[nodiscard]] const_iterator cend() const
  {
    return end();
  }

  /**
   * @brief Returns the number of nodes (including the root node) in the tree.
   * @return Number of nodes (including the root node) in the tree.
   */
  [[nodiscard]] size_t size() const
  {
    auto distance = std::distance(begin(), end());
    return (distance >= 0) ? static_cast<size_t>(distance) : 0ull;
  }

  /**
   * @brief Recursive function to release a subtree using dept-first traversal.
   * @param n The subtree to be released.
   */
  void releaseSubtree(std::unique_ptr<TreeNode<T>>& n)
  {
    while (!n->children().empty()) {
      auto& leaf = n->children();
      while (!leaf[0]->children().empty()) {
        releaseSubtree(leaf[0]);
      }
      leaf.erase(leaf.begin()); // found leaf, drop it
    }
  }

  /**
   * @brief Removed all the children of the root fro, the tree..
   */
  void clear()
  {
    releaseSubtree(_root);
  }

}; // end class class Tree

/**
 * @brief Iterator implementing a depth first pre-order tree traversal.
 *
 * There's two ways we can implement iterators: either we use a stack containing
 * the expanded nodes, or we "manually" backtrack the tree. The first method
 * leads to somewhat faster traversal in theory, since getting to the next node
 * is O(1). On the other hand, we need to add all expanded nodes to the stack,
 * and whenever we copy an iterator, we need to copy the entire stack.
 *
 * STL algorithms are designed around an assumption of fast copying of
 * iterators, so we pick the lighter iterator approach (manually backtracking).
 */
template <typename T>
class Tree<T>::iterator : public std::iterator<std::bidirectional_iterator_tag, T> {

public:
  /**
   * @brief Constructor
   * @param nodePtr pointer to the node
   * @param root pointer to root
   */
  iterator(const TreeNode<T>* nodePtr, const TreeNode<T>* root) : _nodePtr{nodePtr}, _root{root}
  {
  }

  /**
   * @brief Prefix increment operator.
   * @return Iterator to the next node.
   */
  iterator& operator++()
  {
    if (_nodePtr->isLeaf()) {
      // This is a leaf node, so we need to climb up
      for (;;) {
        if (_nodePtr == _root) {
          _nodePtr = nullptr;
          break;
        }
        // note: if _nodePtr is not root, it must have a parent
        const auto& par = _nodePtr->parent();
        // Determine which child we are
        auto next = par->child_begin();
        // no bounds check: nptr is in array
        for (; (*next).get() != _nodePtr; ++next) {
        }
        ++next; // bring to next
        if (next != par->child_end()) {
          // Branch down to next child
          _nodePtr = (*next).get();
          break;
        }
        else {
          // We were the last child of parent node, so continue up
          _nodePtr = par;
        }
      }
    }
    else {
      // Not a leaf node, so move down one step
      _nodePtr = _nodePtr->firstChild();
    }

    return *this;
  }

  /**
   * @brief Prefix decrement operator.
   * @return Iterator to the previous node.
   */
  iterator& operator--()
  {
    if (_nodePtr) {
      // note: -- on first element is undefined => we may safely move up if not
      // left
      if (_nodePtr == _nodePtr->parent()->firstChild()) {
        // nptr is first child => move up
        _nodePtr = _nodePtr->parent();
      }
      else {
        // nptr is not first child => move up one step, then traverse down
        // find pointer from parent
        auto prev = --_nodePtr->parent()->child_end();
        for (; (*prev).get() != _nodePtr; --prev) {
        }
        --prev; // previous from node pointer (prev can't be argv.front())
        _nodePtr = (*prev).get();
        // Now traverse down right most
        while (!_nodePtr->isLeaf()) {
          _nodePtr = _nodePtr->lastChild();
        }
      }
    }
    else {
      // nptr at end, so we need to use root to get to last element
      for (_nodePtr = _root; !_nodePtr->isLeaf();) {
        _nodePtr = _nodePtr->lastChild();
      }
    }
    return *this;
  }

  // Compare two iterators

  bool operator==(const iterator& other) const
  {
    return (_nodePtr == other._nodePtr) && (_root == other._root);
  }
  bool operator!=(const iterator& other) const
  {
    return !(*this == other);
  }

  // Dereferencing

  const TreeNode<T>& operator*() const
  {
    return *_nodePtr;
  }
  const TreeNode<T>* operator->() const
  {
    return _nodePtr;
  }

private:
  const TreeNode<T>* _nodePtr; // pointer to node
  const TreeNode<T>* _root;    // pointer to subtree root

}; // end of class Tree<T>::iterator

template <typename T>
std::size_t TreeNode<T>::uid = 0;

// For debugging
template <typename T>
std::basic_iostream<char>::basic_ostream& operator<<(std::basic_iostream<char>::basic_ostream& out,
                                                     const Tree<T>& tree)
{
  using node_t = std::unique_ptr<TreeNode<T>>;
  std::vector<char> depth;
  std::size_t di = 0;

  const auto push = [&](char c) {
    depth.erase(depth.begin() + static_cast<long>(di), depth.end());
    depth.emplace_back(' ');
    depth.emplace_back(c);
    depth.emplace_back(' ');
    depth.emplace_back(' ');
    depth.emplace_back(0);
    di += 4;
  };

  const auto pop = [&]() { depth[di -= 4] = 0; };

  const std::function<void(const node_t& tree)> print = [&](const node_t& tree) {
    out << "(" << tree->data() << ")\n";
    const auto& children = tree->children();
    for (const auto& child : children) {
      const std::string depthString(depth.begin(), depth.end());
      out << depthString.c_str() << " └─-";
      push(' ');
      print(child);
      pop();
    }
  };

  print(tree.rootPtr());

  return out;
}

} // end of namespace BABYLON

#endif // end of BABYLON_CORE_TREE_H
