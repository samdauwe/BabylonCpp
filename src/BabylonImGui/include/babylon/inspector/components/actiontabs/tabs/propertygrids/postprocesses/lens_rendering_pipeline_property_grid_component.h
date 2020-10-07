#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_LENS_RENDERING_PIPELINE_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_LENS_RENDERING_PIPELINE_PROPERTY_GRID_COMPONENT_H

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/postprocesses/common_rendering_pipeline_property_grid_component.h>
#include <babylon/postprocesses/renderpipeline/pipelines/lens_rendering_pipeline.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT LensRenderingPipelinePropertyGridComponent {

  static void render(const LensRenderingPipelinePtr& renderPipeline)
  {
    // --- GENERAL ---
    CommonRenderingPipelinePropertyGridComponent::render(renderPipeline);
    // --- OPTIONS ---
    static auto optionsContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(optionsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("OPTIONS")) {
      auto sliderChange = SliderLineComponent::render("Edge blur", renderPipeline->edgeBlur(), 0.f,
                                                      5.f, 0.1f, "%.1f");
      if (sliderChange) {
        renderPipeline->edgeBlur = sliderChange.value();
      }
      sliderChange = SliderLineComponent::render(
        "Edge distortion", renderPipeline->edgeDistortion(), 0.f, 5.f, 0.1f, "%.1f");
      if (sliderChange) {
        renderPipeline->edgeDistortion = sliderChange.value();
      }
      sliderChange = SliderLineComponent::render("Grain amount", renderPipeline->grainAmount(), 0.f,
                                                 5.f, 0.1f, "%.1f");
      if (sliderChange) {
        renderPipeline->grainAmount = sliderChange.value();
      }
      sliderChange = SliderLineComponent::render(
        "Chromatic aberration", renderPipeline->chromaticAberration(), 0.f, 5.f, 0.1f, "%.1f");
      if (sliderChange) {
        renderPipeline->chromaticAberration = sliderChange.value();
      }
      sliderChange = SliderLineComponent::render(
        "Darken out of focus", renderPipeline->darkenOutOfFocus(), 0.f, 5.f, 0.1f, "%.1f");
      if (sliderChange) {
        renderPipeline->darkenOutOfFocus = sliderChange.value();
      }
      if (CheckBoxLineComponent::render("Blur noise", renderPipeline->blurNoise())) {
        renderPipeline->blurNoise = !renderPipeline->blurNoise();
      }
      optionsContainerOpened = true;
    }
    else {
      optionsContainerOpened = false;
    }
    // --- DEPTH OF FIELD ---
    static auto depthOfFieldContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(depthOfFieldContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("DEPTH OF FIELD")) {
      auto sliderChange = SliderLineComponent::render("Aperture", renderPipeline->dofAperture(),
                                                      0.f, 10.f, 0.1f, "%.1f");
      if (sliderChange) {
        renderPipeline->dofAperture = sliderChange.value();
      }
      sliderChange = SliderLineComponent::render("Distortion", renderPipeline->dofDistortion(), 0.f,
                                                 10.f, 0.1f, "%.1f");
      if (sliderChange) {
        renderPipeline->dofDistortion = sliderChange.value();
      }
      if (CheckBoxLineComponent::render("Pentagon bokeh", renderPipeline->pentagonBokeh())) {
        renderPipeline->pentagonBokeh = !renderPipeline->pentagonBokeh();
      }
      sliderChange = SliderLineComponent::render("Highlight gain", renderPipeline->highlightsGain(),
                                                 0.f, 5.f, 0.1f, "%.1f");
      if (sliderChange) {
        renderPipeline->highlightsGain = sliderChange.value();
      }
      sliderChange = SliderLineComponent::render(
        "Highlight threshold", renderPipeline->highlightsThreshold(), 0.f, 5.f, 0.1f, "%.1f");
      if (sliderChange) {
        renderPipeline->highlightsThreshold = sliderChange.value();
      }
      depthOfFieldContainerOpened = true;
    }
    else {
      depthOfFieldContainerOpened = false;
    }
  }

}; // end of struct LensRenderingPipelinePropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_LENS_RENDERING_PIPELINE_PROPERTY_GRID_COMPONENT_H
