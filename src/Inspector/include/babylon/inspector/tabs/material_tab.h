#ifndef BABYLON_INSPECTOR_TABS_MATERIAL_TAB_H
#define BABYLON_INSPECTOR_TABS_MATERIAL_TAB_H

#include <babylon/babylon_global.h>
#include <babylon/inspector/tabs/property_tab.h>
#include <babylon/inspector/tree/tree_item.h>

namespace BABYLON {

class Inspector;
class MaterialAdapter;

class BABYLON_SHARED_EXPORT MaterialTab : public PropertyTab {

public:
  MaterialTab(Inspector& inspector);
  ~MaterialTab() override;

protected:
  /** Builds the tree */
  void _buildTree();
  /** Renders the tree panel */
  void _renderTree() override;
  /** Renders the properties panel */
  void _renderProperties() override;

private:
  bool _isInitialized;
  std::vector<TreeItem<MaterialAdapter>> _materials;

}; // end of class MaterialTab

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_TABS_MATERIAL_TAB_H
