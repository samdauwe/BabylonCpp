#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

struct TreeItemSpecializedComponent;
using TreeItemSpecializedComponentPtr
  = std::shared_ptr<TreeItemSpecializedComponent>;

struct BABYLON_SHARED_EXPORT TreeItemComponent {
  char label[128]                           = {"no label"};
  char key[128]                             = {"no key"};
  float offset                              = 0.f;
  bool isExpanded                           = false;
  bool mustExpand                           = false;
  bool isSelected                           = false;
  TreeItemSpecializedComponentPtr component = nullptr;

}; // end of struct TreeItemComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_COMPONENT_H
