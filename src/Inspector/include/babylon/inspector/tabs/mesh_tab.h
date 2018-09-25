#ifndef BABYLON_INSPECTOR_TABS_MESH_TAB_H
#define BABYLON_INSPECTOR_TABS_MESH_TAB_H

#include <babylon/babylon_api.h>
#include <babylon/inspector/tabs/property_tab.h>
#include <babylon/inspector/tree/tree_item.h>

namespace BABYLON {

class MeshAdapter;
class Inspector;

class BABYLON_SHARED_EXPORT MeshTab : public PropertyTab {

public:
  MeshTab(Inspector& inspector);
  ~MeshTab() override;

protected:
  /** Builds the tree */
  void _buildTree();
  /** Renders the tree panel */
  void _renderTree() override;
  /** Renders the properties panel */
  void _renderProperties() override;

private:
  bool _isInitialized;
  std::vector<TreeItem<MeshAdapter>> _meshes;

}; // end of class MeshTab

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_TABS_MESH_TAB_H
