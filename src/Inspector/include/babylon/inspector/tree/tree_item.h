#ifndef BABYLON_INSPECTOR_TREE_TREE_ITEM_H
#define BABYLON_INSPECTOR_TREE_TREE_ITEM_H

#include <memory>
#include <vector>

namespace BABYLON {

class Tab;

template <typename AdapterType>
class TreeItem {

public:
  TreeItem(Tab& tab, std::unique_ptr<AdapterType>&& adapter)
      : _tab{tab}, _adapter{std::move(adapter)}, _active{false}
  {
  }

  TreeItem(const TreeItem& treeitem) = delete;

  TreeItem(TreeItem&& treeitem)
      : children(std::move(treeitem.children))
      , _tab(treeitem._tab)
      , _adapter(std::move(treeitem._adapter))
      , _active{std::move(treeitem._active)}
  {
  }

  TreeItem& operator=(const TreeItem& treeitem) = delete;

  TreeItem& operator=(TreeItem&& treeitem)
  {
    if (&treeitem != this) {
      children = std::move(treeitem.children);
      _tab     = treeitem._tab;
      _adapter = std::move(treeitem._adapter);
      _active  = std::move(treeitem._active);
    }

    return *this;
  }

  ~TreeItem()
  {
  }

  /**
   *@brief Returns the item ID == its adapter ID.
   */
  std::string id() const
  {
    return _adapter->id();
  }

  /**
   *@brief Returns the original adapter.
   */
  AdapterType& adapter()
  {
    return *_adapter;
  }

  /**
   *@brief Set this item as active.
   */
  void active(bool b)
  {
    _active = b;
  }

  /**
   *@brief Set this item as active.
   */
  bool isActive() const
  {
    return _active;
  }

  /**
   * @brief Updates the tree item.
   */
  void update()
  {
  }

  bool operator()(const TreeItem& lhs, const TreeItem& rhs)
  {
    return lhs.id() < rhs.id();
  }

public:
  std::vector<TreeItem<AdapterType>> children;

private:
  // Reference to the tab
  Tab& _tab;
  // The object this item is linked to (should be a primitive or a canvas) TODO
  // should be superclass of all primitives
  std::unique_ptr<AdapterType> _adapter;
  // Whether this item is active
  bool _active;

}; // end of class TreeItem

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_TREE_TREE_ITEM_H
