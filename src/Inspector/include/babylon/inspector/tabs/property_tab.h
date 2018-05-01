#ifndef BABYLON_INSPECTOR_TABS_PROPERTY_TAB_H
#define BABYLON_INSPECTOR_TABS_PROPERTY_TAB_H

#include <babylon/babylon_global.h>
#include <babylon/inspector/tabs/tab.h>

namespace BABYLON {

class Inspector;
template <typename T>
class TreeItem;

/**
 * @brief A Property tab can creates two panels:
 * a tree panel and a detail panel,
 * in which properties will be displayed.
 * Both panels are separated by a resize bar
 */
class BABYLON_SHARED_EXPORT PropertyTab : public Tab {

public:
  PropertyTab(const string_t& name, Inspector& inspector);
  ~PropertyTab() override;

  void render() override;
  void dispose() override;

  /**
   * @brief Toggles the selection of an item in the tree.
   */
  template <typename T>
  void toggleSelection(TreeItem<T>& item, std::vector<TreeItem<T>>& treeItems)
  {
    if (item.isActive()) {
      // Deactivate the node
      item.active(false);
    }
    else {
      // Active the node
      activateNode(item, treeItems);
    }
  }

  /**
   * @brief Set the given item as active in the tree.
   */
  template <typename T>
  void activateNode(TreeItem<T>& item, std::vector<TreeItem<T>>& treeItems)
  {
    if (!treeItems.empty()) {
      for (auto& treeItem : treeItems) {
        treeItem.active(false);
      }
    }
    item.active(true);
  }

protected:
  /** Renders the tree panel */
  virtual void _renderTree() = 0;
  /** Renders the properties panel */
  virtual void _renderProperties() = 0;

protected:
  Inspector& _inspector;

}; // end of class PropertyTab

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_TABS_PROPERTY_TAB_H
