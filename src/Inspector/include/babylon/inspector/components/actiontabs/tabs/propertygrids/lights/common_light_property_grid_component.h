#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_COMMON_LIGHT_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_COMMON_LIGHT_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Light;
using LightPtr = std::shared_ptr<Light>;

struct ICommonLightPropertyGridComponentProps {
  LightPtr light;
}; // end of ICommonLightPropertyGridComponentProps

class BABYLON_SHARED_EXPORT CommonLightPropertyGridComponent {

public:
  CommonLightPropertyGridComponent(
    const ICommonLightPropertyGridComponentProps& props);
  ~CommonLightPropertyGridComponent();

  void render();

public:
  ICommonLightPropertyGridComponentProps props;

}; // end of class CommonLightPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_COMMON_LIGHT_PROPERTY_GRID_COMPONENT_H
