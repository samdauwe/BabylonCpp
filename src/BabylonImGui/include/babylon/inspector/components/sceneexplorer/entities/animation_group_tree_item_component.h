#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_ANIMATION_GROUP_TREE_ITEM_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_ANIMATION_GROUP_TREE_ITEM_COMPONENT_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_specialized_component.h>
#include <imgui_utils/icons_font_awesome_5.h>

namespace BABYLON {

class AnimationGroup;
using AnimationGroupPtr = std::shared_ptr<AnimationGroup>;

struct IAnimationGroupTreeItemComponentProps {
  AnimationGroupPtr animationGroup = nullptr;
  std::function<void()> onClick    = nullptr;
}; // end of struct IAnimationGroupTreeItemComponentProps

class BABYLON_SHARED_EXPORT AnimationGroupTreeItemComponent
    : public TreeItemSpecializedComponent {

public:
  /** Font Awesome Icon Unicodes **/
  static constexpr const char* faFilm = ICON_FA_FILM;

public:
  AnimationGroupTreeItemComponent(
    const IAnimationGroupTreeItemComponentProps& props);
  virtual ~AnimationGroupTreeItemComponent() override;

  void render() override;

public:
  IAnimationGroupTreeItemComponentProps props;

}; // end of class AnimationGroupTreeItemComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_ANIMATION_GROUP_TREE_ITEM_COMPONENT_H
