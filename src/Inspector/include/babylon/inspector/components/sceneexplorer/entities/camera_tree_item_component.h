#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_CAMERA_TREE_ITEM_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_CAMERA_TREE_ITEM_COMPONENT_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/imgui/icons_font_awesome_5.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class Camera;
class Scene;
using CameraPtr = std::shared_ptr<Camera>;

struct ICameraTreeItemComponentProps {
  CameraPtr camera              = nullptr;
  std::function<void()> onClick = nullptr;
}; // end of struct ICameraTreeItemComponentProps

struct CameraTreeItemComponentState {
  bool isActive = false;
}; // end of struct CameraTreeItemComponentState

class BABYLON_SHARED_EXPORT CameraTreeItemComponent {

public:
  /** Font Awesome Icon Unicodes **/
  // fa-video-camera
  static constexpr const char* faVideo = ICON_FA_VIDEO;
  // fa-camera
  static constexpr const char* faCamera = ICON_FA_CAMERA;

public:
  CameraTreeItemComponent(const ICameraTreeItemComponentProps& props);
  ~CameraTreeItemComponent();

  void setActive();
  void componentWillMount();
  void componentWillUnmount();
  void render();

public:
  ICameraTreeItemComponentProps props;
  CameraTreeItemComponentState state;

private:
  Observer<Scene>::Ptr _onActiveCameraObserver;

}; // end of class CameraTreeItemComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_CAMERA_TREE_ITEM_COMPONENT_H
