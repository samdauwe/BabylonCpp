#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_SSAO2_RENDERING_PIPELINE_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_SSAO2_RENDERING_PIPELINE_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/cameras/camera.h>
#include <babylon/engines/scene.h>
#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/postprocesses/common_rendering_pipeline_property_grid_component.h>
#include <babylon/postprocesses/renderpipeline/pipelines/ssao2_rendering_pipeline.h>

namespace BABYLON {

class SSAO2RenderingPipeline;
using SSAO2RenderingPipelinePtr = std::shared_ptr<SSAO2RenderingPipeline>;

struct BABYLON_SHARED_EXPORT SSAO2RenderingPipelinePropertyGridComponent {

  static void render(const SSAO2RenderingPipelinePtr& renderPipeline)
  {
    const auto& camera = renderPipeline->scene()->activeCamera;

    // --- GENERAL ---
    CommonRenderingPipelinePropertyGridComponent::render(renderPipeline);
    // --- SSAO ---
    static auto ssaoContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(ssaoContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("SSAO")) {
      auto sliderChange = SliderLineComponent::render(
        "Strength", renderPipeline->totalStrength, 0.f, 2.f, 0.05f, "%.2f");
      if (sliderChange) {
        renderPipeline->totalStrength = sliderChange.value();
      }
      sliderChange = SliderLineComponent::render("Base", renderPipeline->base,
                                                 0.f, 1.f, 0.05f, "%.2f");
      if (sliderChange) {
        renderPipeline->base = sliderChange.value();
      }
      if (camera) {
        sliderChange = SliderLineComponent::render(
          "Max Z", renderPipeline->maxZ, 0.f, camera->maxZ, 1.f, "%.0f");
        if (sliderChange) {
          renderPipeline->maxZ = sliderChange.value();
        }
      }
      sliderChange = SliderLineComponent::render(
        "Min Z aspect", renderPipeline->minZAspect, 0.f, 0.5f, 0.01f, "%.2f");
      if (sliderChange) {
        renderPipeline->minZAspect = sliderChange.value();
      }
      sliderChange = SliderLineComponent::render(
        "Radius", renderPipeline->radius, 0.f, 10.f, 0.05f, "%.2f");
      if (sliderChange) {
        renderPipeline->radius = sliderChange.value();
      }
      ssaoContainerOpened = true;
    }
    else {
      ssaoContainerOpened = false;
    }
  }

}; // end of struct SSAO2RenderingPipelinePropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_LENS_RENDERING_PIPELINE_PROPERTY_GRID_COMPONENT_H
