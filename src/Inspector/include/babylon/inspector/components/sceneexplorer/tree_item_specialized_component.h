#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_SPECIALIZED_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_SPECIALIZED_COMPONENT_H

#include <optional>

#include <babylon/babylon_api.h>

#include <babylon/inspector/components/sceneexplorer/tree_item_component.h>

namespace BABYLON {

struct SpecializedComponentInfo {
  EntityType entityType          = EntityType::Unknown;
  std::optional<size_t> uniqueId = std::nullopt;
}; // end of struct SpecializedComponentInfo

struct BABYLON_SHARED_EXPORT TreeItemSpecializedComponent {

  virtual void render() = 0;

  // Label to be rendered in the item tree
  char label[128] = {"no label"};
  // Used identify the specialized component
  SpecializedComponentInfo specializedComponentInfo{};

}; // end of struct TreeItemSpecializedComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_SPECIALIZED_COMPONENT_H
