#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_COMMON_RENDERING_PIPELINE_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_COMMON_RENDERING_PIPELINE_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/text_line_component.h>
#include <babylon/postprocesses/renderpipeline/pipelines/default_rendering_pipeline.h>
#include <babylon/postprocesses/renderpipeline/pipelines/ssao2_rendering_pipeline.h>
#include <babylon/postprocesses/renderpipeline/pipelines/standard_rendering_pipeline.h>
#include <babylon/postprocesses/renderpipeline/post_process_render_pipeline.h>

namespace BABYLON {

class PostProcessRenderPipeline;
using PostProcessRenderPipelinePtr = std::shared_ptr<PostProcessRenderPipeline>;

struct BABYLON_SHARED_EXPORT CommonRenderingPipelinePropertyGridComponent {

  static void render(const PostProcessRenderPipelinePtr& renderPipeline)
  {
    // --- GENERAL ---
    static auto generalContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(generalContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("GENERAL")) {
      const auto className = renderPipeline->getClassName();
      TextLineComponent::render("Class", renderPipeline->getClassName());
      if (className == "DefaultRenderingPipeline") {
        auto defaultRenderingPipeline
          = std::static_pointer_cast<DefaultRenderingPipeline>(renderPipeline);
        auto sliderChange = SliderLineComponent::render(
          "Samples", defaultRenderingPipeline->samples(), 1.f, 8.f, 1.f,
          "%.0f");
        if (sliderChange) {
          defaultRenderingPipeline->samples
            = static_cast<unsigned>(sliderChange.value());
        }
      }
      else if (className == "SSAO2RenderingPipeline") {
        auto ssao2RenderingPipeline
          = std::static_pointer_cast<SSAO2RenderingPipeline>(renderPipeline);
        auto sliderChange = SliderLineComponent::render(
          "Samples", ssao2RenderingPipeline->samples(), 1.f, 8.f, 1.f, "%.0f");
        if (sliderChange) {
          ssao2RenderingPipeline->samples
            = static_cast<unsigned>(sliderChange.value());
        }
      }
      else if (className == "StandardRenderingPipeline") {
        auto standardRenderingPipeline
          = std::static_pointer_cast<StandardRenderingPipeline>(renderPipeline);
        auto sliderChange = SliderLineComponent::render(
          "Samples", standardRenderingPipeline->samples(), 1.f, 8.f, 1.f,
          "%.0f");
        if (sliderChange) {
          standardRenderingPipeline->samples
            = static_cast<unsigned>(sliderChange.value());
        }
      }
      generalContainerOpened = true;
    }
    else {
      generalContainerOpened = false;
    }
  }

}; // end of struct CommonRenderingPipelinePropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_COMMON_RENDERING_PIPELINE_PROPERTY_GRID_COMPONENT_H
