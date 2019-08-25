#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_SPECIALIZED_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_SPECIALIZED_COMPONENT_H

#include <babylon/babylon_api.h>

#include <babylon/inspector/components/sceneexplorer/tree_item_component.h>
#include <babylon/inspector/entity.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT TreeItemSpecializedComponent {
  virtual ~TreeItemSpecializedComponent() = default;
  virtual void render() = 0;

  // Label to be rendered in the item tree
  char label[128] = {"no label"};
  // Used to identify the entity contained in the component
  EntityInfo entityInfo{};

}; // end of struct TreeItemSpecializedComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_SPECIALIZED_COMPONENT_H
