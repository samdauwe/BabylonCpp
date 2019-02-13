#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_FREE_CAMERA_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_FREE_CAMERA_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class CommonCameraPropertyGridComponent;
class FreeCamera;
using FreeCameraPtr = std::shared_ptr<FreeCamera>;

struct IFreeCameraPropertyGridComponentProps {
  FreeCameraPtr camera;
}; // end of IFreeCameraPropertyGridComponentProps

class BABYLON_SHARED_EXPORT FreeCameraPropertyGridComponent {

public:
  FreeCameraPropertyGridComponent(
    const IFreeCameraPropertyGridComponentProps& props);
  ~FreeCameraPropertyGridComponent();

  void render();

public:
  IFreeCameraPropertyGridComponentProps props;

private:
  std::unique_ptr<CommonCameraPropertyGridComponent>
    _commonCameraPropertyGridComponent;

}; // end of class FreeCameraPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_FREE_CAMERA_PROPERTY_GRID_COMPONENT_H
