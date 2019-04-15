#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_LIGHT_TREE_ITEM_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_LIGHT_TREE_ITEM_COMPONENT_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/imgui/icons_font_awesome_5.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_specialized_component.h>

namespace BABYLON {

class Light;
class Scene;
using LightPtr = std::shared_ptr<Light>;

struct ILightTreeItemComponentProps {
  LightPtr light                = nullptr;
  std::function<void()> onClick = nullptr;
}; // end of struct ILightTreeItemComponentProps

struct LigtTreeItemComponentState {
  bool isEnabled      = false;
  bool isGizmoEnabled = false;
}; // end of struct CameraTreeItemComponentState

class BABYLON_SHARED_EXPORT LightTreeItemComponent
    : public TreeItemSpecializedComponent {

public:
  /** Font Awesome Icon Unicodes **/
  static constexpr const char* faLightbulb       = ICON_FA_LIGHTBULB;
  static constexpr const char* faLightbubRegular = ICON_FA_LIGHTBULB;
  static constexpr const char* faEye             = ICON_FA_EYE;

public:
  LightTreeItemComponent(const ILightTreeItemComponentProps& props);
  virtual ~LightTreeItemComponent();

  void switchIsEnabled();
  void toggleGizmo();
  void render() override;

public:
  ILightTreeItemComponentProps props;
  LigtTreeItemComponentState state;

}; // end of class LightTreeItemComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_LIGHT_TREE_ITEM_COMPONENT_H
