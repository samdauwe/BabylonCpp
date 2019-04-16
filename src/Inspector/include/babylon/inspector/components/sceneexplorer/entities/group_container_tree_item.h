#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_GROUP_CONTAINER_TREE_ITEM_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_GROUP_CONTAINER_TREE_ITEM_COMPONENT_H

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_specialized_component.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT GroupContainerTreeItemComponent
    : public TreeItemSpecializedComponent {

public:
  GroupContainerTreeItemComponent(const char* label);
  virtual ~GroupContainerTreeItemComponent();

  void renderLabelWithIcon() override;
  void renderControls() override;

}; // end of class CameraTreeItemComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_GROUP_CONTAINER_TREE_ITEM_COMPONENT_H
