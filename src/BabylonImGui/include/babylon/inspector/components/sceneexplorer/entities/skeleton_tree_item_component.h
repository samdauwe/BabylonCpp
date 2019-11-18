#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_SKELETON_TREE_ITEM_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_SKELETON_TREE_ITEM_COMPONENT_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_specialized_component.h>
#include <imgui_utils/icons_font_awesome_5.h>

namespace BABYLON {

class Skeleton;
using SkeletonPtr = std::shared_ptr<Skeleton>;

struct ISkeletonTreeItemComponentProps {
  SkeletonPtr skeleton          = nullptr;
  std::function<void()> onClick = nullptr;
}; // end of struct ISkeletonTreeItemComponentProps

class BABYLON_SHARED_EXPORT SkeletonTreeItemComponent : public TreeItemSpecializedComponent {

public:
  /** Font Awesome Icon Unicodes **/
  static constexpr const char* faSkull = ICON_FA_SKULL;

public:
  SkeletonTreeItemComponent(const ISkeletonTreeItemComponentProps& props);
  ~SkeletonTreeItemComponent() override; // = default

  void render() override;

public:
  ISkeletonTreeItemComponentProps props;

}; // end of class SkeletonTreeItemComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_SKELETON_TREE_ITEM_COMPONENT_H
