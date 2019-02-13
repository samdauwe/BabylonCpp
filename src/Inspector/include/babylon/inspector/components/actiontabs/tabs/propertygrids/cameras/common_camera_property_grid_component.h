#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_COMMON_CAMERA_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_COMMON_CAMERA_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Camera;
using CameraPtr = std::shared_ptr<Camera>;

struct ICommonCameraPropertyGridComponentProps {
  CameraPtr camera;
}; // end of ICommonCameraPropertyGridComponentProps

struct CommonCameraPropertyGridComponentState {
  unsigned int mode = 0;
}; // end of CommonCameraPropertyGridComponentState

class BABYLON_SHARED_EXPORT CommonCameraPropertyGridComponent {

public:
  CommonCameraPropertyGridComponent(
    const ICommonCameraPropertyGridComponentProps& props);
  ~CommonCameraPropertyGridComponent();

  void render();

public:
  ICommonCameraPropertyGridComponentProps props;
  CommonCameraPropertyGridComponentState state;

}; // end of class CommonCameraPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_COMMON_CAMERA_PROPERTY_GRID_COMPONENT_H
