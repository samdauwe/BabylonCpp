#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_MATERIAL_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_MATERIAL_PROPERTY_GRID_COMPONENT_H

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/materials/common_material_property_grid_component.h>
#include <babylon/materials/material.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT MaterialPropertyGridComponent {

  static void render(const MaterialPtr& material)
  {
    CommonMaterialPropertyGridComponent::render(material);
  }

}; // end of struct MaterialPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_MATERIAL_PROPERTY_GRID_COMPONENT_H
