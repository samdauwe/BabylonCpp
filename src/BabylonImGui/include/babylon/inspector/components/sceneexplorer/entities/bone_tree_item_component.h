#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_BONE_TREE_ITEM_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_BONE_TREE_ITEM_COMPONENT_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_specialized_component.h>
#include <imgui_utils/icons_font_awesome_5.h>

namespace BABYLON {

class Bone;
using BonePtr = std::shared_ptr<Bone>;

struct IBoneTreeItemComponentProps {
  BonePtr bone                  = nullptr;
  std::function<void()> onClick = nullptr;
}; // end of struct IBoneTreeItemComponentProps

class BABYLON_SHARED_EXPORT BoneTreeItemComponent
    : public TreeItemSpecializedComponent {

public:
  /** Font Awesome Icon Unicodes **/
  static constexpr const char* faBone = ICON_FA_BONE;

public:
  BoneTreeItemComponent(const IBoneTreeItemComponentProps& props);
  virtual ~BoneTreeItemComponent() override;

  void render() override;

public:
  IBoneTreeItemComponentProps props;

}; // end of class BoneTreeItemComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_BONE_TREE_ITEM_COMPONENT_H
