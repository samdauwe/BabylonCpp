#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_SSAO_RENDERING_PIPELINE_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_SSAO_RENDERING_PIPELINE_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/postprocesses/common_rendering_pipeline_property_grid_component.h>
#include <babylon/postprocesses/renderpipeline/pipelines/ssao_rendering_pipeline.h>

namespace BABYLON {

class SSAORenderingPipeline;
using SSAORenderingPipelinePtr = std::shared_ptr<SSAORenderingPipeline>;

struct BABYLON_SHARED_EXPORT SSAORenderingPipelinePropertyGridComponent {

  static void render(const SSAORenderingPipelinePtr& renderPipeline)
  {
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
      sliderChange = SliderLineComponent::render(
        "Radius", renderPipeline->radius, 0.f, 10.f, 0.05f, "%.2f");
      if (sliderChange) {
        renderPipeline->radius = sliderChange.value();
      }
      sliderChange = SliderLineComponent::render(
        "Area", renderPipeline->area, 0.0001f, 0.01f, 0.0001f, "%.4f");
      if (sliderChange) {
        renderPipeline->area = sliderChange.value();
      }
      sliderChange = SliderLineComponent::render(
        "Falloff", renderPipeline->fallOff, 0.f, 0.00001f, 0.000001f, "%.6f");
      if (sliderChange) {
        renderPipeline->fallOff = sliderChange.value();
      }
      ssaoContainerOpened = true;
    }
    else {
      ssaoContainerOpened = false;
    }
  }

}; // end of struct SSAORenderingPipelinePropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_LENS_RENDERING_PIPELINE_PROPERTY_GRID_COMPONENT_H
