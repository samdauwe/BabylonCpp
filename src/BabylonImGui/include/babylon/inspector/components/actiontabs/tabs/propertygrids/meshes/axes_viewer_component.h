#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_AXES_VIEWER_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_AXES_VIEWER_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>

namespace BABYLON {

class TransformNode;
using TransformNodePtr = std::shared_ptr<TransformNode>;

struct BABYLON_SHARED_EXPORT AxesViewerComponent {

  static bool displayAxis(const TransformNodePtr& /*node*/)
  {
    return false;
  }

  static void displayAxes(const TransformNodePtr& /*node*/)
  {
  }

  static void render(const TransformNodePtr& node)
  {
    if (CheckBoxLineComponent::render("Display axes", displayAxis(node))) {
    }
  }

}; // end of struct AxesViewerComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_AXES_VIEWER_COMPONENT_H
