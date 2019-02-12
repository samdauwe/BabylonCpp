#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_HEMISPHERIC_LIGHT_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_HEMISPHERIC_LIGHT_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class CommonLightPropertyGridComponent;
class HemisphericLight;
using HemisphericLightPtr = std::shared_ptr<HemisphericLight>;

struct IHemisphericLightPropertyGridComponentProps {
  HemisphericLightPtr light;
}; // end of IHemisphericLightPropertyGridComponentProps

class BABYLON_SHARED_EXPORT HemisphericLightPropertyGridComponent {

public:
  HemisphericLightPropertyGridComponent(
    const IHemisphericLightPropertyGridComponentProps& props);
  ~HemisphericLightPropertyGridComponent();

  void render();

public:
  IHemisphericLightPropertyGridComponentProps props;

private:
  std::unique_ptr<CommonLightPropertyGridComponent>
    _commonLightPropertyGridComponent;

}; // end of class HemisphericLightPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LIGHTS_HEMISPHERIC_LIGHT_PROPERTY_GRID_COMPONENT_H
