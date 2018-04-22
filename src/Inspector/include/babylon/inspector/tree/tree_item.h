#ifndef BABYLON_INSPECTOR_TREE_TREE_ITEM_H
#define BABYLON_INSPECTOR_TREE_TREE_ITEM_H

#include <babylon/babylon_stl.h>

namespace BABYLON {

class Tab;

template <typename AdapterType>
class TreeItem {

public:
  TreeItem(Tab& tab, unique_ptr_t<AdapterType>&& adapter)
      : _tab{tab}, _adapter{::std::move(adapter)}, _active{false}
  {
  }

  TreeItem(const TreeItem& treeitem) = delete;

  TreeItem(TreeItem&& treeitem)
      : _tab(treeitem._tab)
      , _adapter(::std::move(treeitem._adapter))
      , _active{::std::move(treeitem._active)}
  {
  }

  ~TreeItem()
  {
  }

  /**
   *@brief Returns the item ID == its adapter ID.
   */
  string_t id() const
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

  bool operator()(const TreeItem& lhs, const TreeItem& rhs)
  {
    return lhs.id() < rhs.id();
  }

private:
  // Reference to the tab
  Tab& _tab;
  // The object this item is linked to (should be a primitive or a canvas) TODO
  // should be superclass of all primitives
  unique_ptr_t<AdapterType> _adapter;
  // Whether this item is active
  bool _active;

}; // end of class TreeItem

} // end of namespace BABYLON

#endif // BABYLON_INSPECTOR_TREE_TREE_ITEM_H
