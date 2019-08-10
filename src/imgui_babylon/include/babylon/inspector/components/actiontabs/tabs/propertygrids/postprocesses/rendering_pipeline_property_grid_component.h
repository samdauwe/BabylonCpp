#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_RENDERING_PIPELINE_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_RENDERING_PIPELINE_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/postprocesses/common_rendering_pipeline_property_grid_component.h>

namespace BABYLON {

class PostProcessRenderPipeline;
using PostProcessRenderPipelinePtr = std::shared_ptr<PostProcessRenderPipeline>;

struct BABYLON_SHARED_EXPORT RenderingPipelinePropertyGridComponent {

  static void render(const PostProcessRenderPipelinePtr& renderPipeline)
  {
    // --- GENERAL ---
    CommonRenderingPipelinePropertyGridComponent::render(renderPipeline);
  }

}; // end of struct RenderingPipelinePropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_RENDERING_PIPELINE_PROPERTY_GRID_COMPONENT_H
