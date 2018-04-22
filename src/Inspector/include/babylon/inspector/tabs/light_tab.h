#ifndef BABYLON_INSPECTOR_TABS_LIGHT_TAB_H
#define BABYLON_INSPECTOR_TABS_LIGHT_TAB_H

#include <babylon/babylon_global.h>
#include <babylon/inspector/tabs/property_tab.h>
#include <babylon/inspector/tree/tree_item.h>

namespace BABYLON {

class Light;
class LightAdapter;
class Inspector;

class BABYLON_SHARED_EXPORT LightTab : public PropertyTab {

public:
  LightTab(Inspector& inspector);
  ~LightTab() override;

protected:
  /** Builds the tree */
  void _buildTree();
  /** Renders the tree panel */
  void _renderTree() override;

private:
  bool _isInitialized;
  std::vector<TreeItem<LightAdapter>> _lights;

}; // end of class LightTab

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_TABS_LIGHT_TAB_H
