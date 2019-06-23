#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_POST_PROCESS_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_POST_PROCESS_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/postprocesses/common_post_process_property_grid_component.h>

namespace BABYLON {

class PostProcess;
using PostProcessPtr = std::shared_ptr<PostProcess>;

struct BABYLON_SHARED_EXPORT PostProcessPropertyGridComponent {

  static void render(const PostProcessPtr& postProcess)
  {
    // --- GENERAL ---
    CommonPostProcessPropertyGridComponent::render(postProcess);
  }

}; // end of struct PostProcessPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_POST_PROCESS_PROPERTY_GRID_COMPONENT_H
