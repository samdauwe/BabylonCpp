#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_SPECIALIZED_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_SPECIALIZED_COMPONENT_H

#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT TreeItemSpecializedComponent {

  virtual void renderLabelWithIcon() = 0;
  virtual void renderControls()      = 0;

  char label[64] = {"no name"};
  std::string labelWithoutIcon;
  bool isActive = false;

}; // end of struct TreeItemSpecializedComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_SPECIALIZED_COMPONENT_H
