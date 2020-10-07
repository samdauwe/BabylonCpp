#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_DEFAULT_RENDERING_PIPELINE_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_DEFAULT_RENDERING_PIPELINE_PROPERTY_GRID_COMPONENT_H

#include <babylon/babylon_api.h>
#include <babylon/cameras/camera.h>
#include <babylon/engines/scene.h>
#include <babylon/inspector/components/actiontabs/lines/button_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/color3_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/options_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/vector2_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/postprocesses/common_rendering_pipeline_property_grid_component.h>
#include <babylon/layers/glow_layer.h>
#include <babylon/materials/image_processing_configuration.h>
#include <babylon/postprocesses/chromatic_aberration_post_process.h>
#include <babylon/postprocesses/depth_of_field_effect.h>
#include <babylon/postprocesses/grain_post_process.h>
#include <babylon/postprocesses/image_processing_post_process.h>
#include <babylon/postprocesses/renderpipeline/pipelines/default_rendering_pipeline.h>
#include <babylon/postprocesses/sharpen_post_process.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT DefaultRenderingPipelinePropertyGridComponent {

  static void render(const DefaultRenderingPipelinePtr& renderPipeline)
  {
    const auto& camera = renderPipeline->scene()->activeCamera();

    static std::vector<std::pair<const char*, unsigned int>> toneMappingOptions{
      {"Standard", ImageProcessingConfiguration::TONEMAPPING_STANDARD},
      {"ACES", ImageProcessingConfiguration::TONEMAPPING_ACES}};

    static std::vector<std::pair<const char*, unsigned int>> vignetteModeOptions{
      {"Multiply", ImageProcessingConfiguration::VIGNETTEMODE_MULTIPLY()},
      {"Opaque", ImageProcessingConfiguration::VIGNETTEMODE_OPAQUE()}};

    // --- GENERAL ---
    CommonRenderingPipelinePropertyGridComponent::render(renderPipeline);
    // --- BLOOM ---
    static auto bloomContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(bloomContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("BLOOM")) {
      if (CheckBoxLineComponent::render("Enabled", renderPipeline->bloomEnabled())) {
        renderPipeline->bloomEnabled = !renderPipeline->bloomEnabled();
      }
      if (renderPipeline->bloomEnabled()) {
        auto sliderChange = SliderLineComponent::render(
          "Threshold", renderPipeline->bloomThreshold(), 0.f, 1.f, 0.05f, "%.2f");
        if (sliderChange) {
          renderPipeline->bloomThreshold = sliderChange.value();
        }
        sliderChange = SliderLineComponent::render("Weight", renderPipeline->bloomWeight(), 0.f,
                                                   1.f, 0.05f, "%.2f");
        if (sliderChange) {
          renderPipeline->bloomWeight = sliderChange.value();
        }
        sliderChange = SliderLineComponent::render("Kernel", renderPipeline->bloomKernel(), 0.f,
                                                   128.f, 1.f, "%.0f");
        if (sliderChange) {
          renderPipeline->bloomKernel = sliderChange.value();
        }
        sliderChange = SliderLineComponent::render("Scale", renderPipeline->bloomScale(), 0.f, 1.f,
                                                   0.25f, "%.2f");
        if (sliderChange) {
          renderPipeline->bloomScale = sliderChange.value();
        }
      }
      bloomContainerOpened = true;
    }
    else {
      bloomContainerOpened = false;
    }
    // --- CHROMATIC ABERRATION ---
    static auto chromaticAberrationContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(chromaticAberrationContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("CHROMATIC ABERRATION")) {
      if (CheckBoxLineComponent::render("Enabled", renderPipeline->chromaticAberrationEnabled())) {
        renderPipeline->chromaticAberrationEnabled = !renderPipeline->chromaticAberrationEnabled();
      }
      if (renderPipeline->chromaticAberrationEnabled()) {
        auto sliderChange = SliderLineComponent::render(
          "aberrationAmount", renderPipeline->chromaticAberration->aberrationAmount, 0.f, 128.f,
          0.1f, "%.1f");
        if (sliderChange) {
          renderPipeline->chromaticAberration->aberrationAmount = sliderChange.value();
        }
        sliderChange = SliderLineComponent::render(
          "Radial intensity", renderPipeline->chromaticAberration->radialIntensity, 0.f, 1.f, 0.01f,
          "%.2f");
        if (sliderChange) {
          renderPipeline->chromaticAberration->radialIntensity = sliderChange.value();
        }
        Vector2LineComponent::render("Center", renderPipeline->chromaticAberration->centerPosition);
        Vector2LineComponent::render("Direction", renderPipeline->chromaticAberration->direction);
      }
      chromaticAberrationContainerOpened = true;
    }
    else {
      chromaticAberrationContainerOpened = false;
    }
    // --- DEPTH OF FIELD ---
    static auto deptOfFieldContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(deptOfFieldContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("DEPTH OF FIELD")) {
      if (CheckBoxLineComponent::render("Enabled", renderPipeline->depthOfFieldEnabled())) {
        renderPipeline->depthOfFieldEnabled = !renderPipeline->depthOfFieldEnabled();
      }
      if (renderPipeline->depthOfFieldEnabled()) {
        auto sliderChange
          = SliderLineComponent::render("Focal length", renderPipeline->depthOfField->focalLength(),
                                        0.f, camera->maxZ, 1.f, "%.1f");
        if (sliderChange) {
          renderPipeline->depthOfField->focalLength = sliderChange.value();
        }
        sliderChange = SliderLineComponent::render("fStop", renderPipeline->depthOfField->fStop(),
                                                   0.f, 32.f, 0.1f, "%.1f");
        if (sliderChange) {
          renderPipeline->depthOfField->fStop = sliderChange.value();
        }
        sliderChange
          = SliderLineComponent::render("Distance", renderPipeline->depthOfField->focusDistance(),
                                        0.f, camera->maxZ, 0.1f, "%.1f");
        if (sliderChange) {
          renderPipeline->depthOfField->focusDistance = sliderChange.value();
        }
        sliderChange = SliderLineComponent::render(
          "Lens size", renderPipeline->depthOfField->lensSize(), 0.f, 1000.f, 1.f, "%.0f");
        if (sliderChange) {
          renderPipeline->depthOfField->lensSize = sliderChange.value();
        }
      }
      deptOfFieldContainerOpened = true;
    }
    else {
      deptOfFieldContainerOpened = false;
    }
    // --- FXAA ---
    static auto fxaaContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(fxaaContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("FXAA")) {
      if (CheckBoxLineComponent::render("Enabled", renderPipeline->fxaaEnabled())) {
        renderPipeline->fxaaEnabled = !renderPipeline->fxaaEnabled();
      }
      fxaaContainerOpened = true;
    }
    else {
      fxaaContainerOpened = false;
    }
    // --- GLOW LAYER ---
    static auto glowLayerContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(glowLayerContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("GLOW LAYER")) {
      if (CheckBoxLineComponent::render("Enabled", renderPipeline->glowLayerEnabled())) {
        renderPipeline->glowLayerEnabled = !renderPipeline->glowLayerEnabled();
      }
      if (renderPipeline->glowLayerEnabled()) {
        auto sliderChange = SliderLineComponent::render(
          "Blur kernel size", renderPipeline->glowLayer()->blurKernelSize(), 1.f, 128.f, 1.f,
          "%.0f");
        if (sliderChange) {
          renderPipeline->glowLayer()->blurKernelSize = sliderChange.value();
        }
        sliderChange = SliderLineComponent::render(
          "Intensity", renderPipeline->glowLayer()->intensity(), 0.f, 10.f, 0.1f, "%.1f");
        if (sliderChange) {
          renderPipeline->glowLayer()->intensity = sliderChange.value();
        }
      }
      glowLayerContainerOpened = true;
    }
    else {
      glowLayerContainerOpened = false;
    }
    // --- GRAIN ---
    static auto grainContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(grainContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("GRAIN")) {
      if (CheckBoxLineComponent::render("Enabled", renderPipeline->grainEnabled())) {
        renderPipeline->grainEnabled = !renderPipeline->grainEnabled();
      }
      if (renderPipeline->grainEnabled()) {
        if (CheckBoxLineComponent::render("Animated", renderPipeline->grain->animated)) {
          renderPipeline->grain->animated = !renderPipeline->grain->animated;
        }
        auto sliderChange = SliderLineComponent::render(
          "Intensity", renderPipeline->grain->intensity, 0.f, 50.f, 0.1f, "%.1f");
        if (sliderChange) {
          renderPipeline->grain->intensity = sliderChange.value();
        }
      }
      grainContainerOpened = true;
    }
    else {
      grainContainerOpened = false;
    }
    // --- IMAGE PROCESSING ---
    static auto imageProcessingContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(imageProcessingContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("IMAGE PROCESSING")) {
      if (CheckBoxLineComponent::render("Enabled", renderPipeline->imageProcessingEnabled())) {
        renderPipeline->imageProcessingEnabled = !renderPipeline->imageProcessingEnabled();
      }
      const auto imageProcessing
        = std::static_pointer_cast<ImageProcessingPostProcess>(renderPipeline->imageProcessing);
      if (imageProcessing) {
        if (ButtonLineComponent::render("Convert clear color to linear")) {
          renderPipeline->scene()->clearColor = renderPipeline->scene()->clearColor.toLinearSpace();
        }
        if (ButtonLineComponent::render("Convert clear color to gamma")) {
          renderPipeline->scene()->clearColor = renderPipeline->scene()->clearColor.toGammaSpace();
        }
        auto sliderChange = SliderLineComponent::render("Contrast", imageProcessing->contrast(),
                                                        0.f, 4.f, 0.1f, "%.2f");
        if (sliderChange) {
          imageProcessing->contrast = sliderChange.value();
        }
        sliderChange = SliderLineComponent::render("Exposure", imageProcessing->exposure(), 0.f,
                                                   4.f, 0.1f, "%.2f");
        if (sliderChange) {
          imageProcessing->exposure = sliderChange.value();
        }
        if (CheckBoxLineComponent::render("Tone mapping", imageProcessing->toneMappingEnabled())) {
          imageProcessing->toneMappingEnabled = !imageProcessing->toneMappingEnabled();
        }
        auto optionChange = OptionsLineComponent::render(
          "Tone mapping type", imageProcessing->toneMappingType(), toneMappingOptions);
        if (optionChange) {
          imageProcessing->toneMappingType = optionChange.value();
        }
        if (CheckBoxLineComponent::render("Vignette", imageProcessing->vignetteEnabled())) {
          imageProcessing->vignetteEnabled = !imageProcessing->vignetteEnabled();
        }
        sliderChange = SliderLineComponent::render(
          "Vignette weight", imageProcessing->vignetteWeight, 0.f, 4.f, 0.1f, "%.2f");
        if (sliderChange) {
          imageProcessing->vignetteWeight = sliderChange.value();
        }
        sliderChange = SliderLineComponent::render(
          "Vignette stretch", imageProcessing->vignetteStretch, 0.f, 1.f, 0.1f, "%.2f");
        if (sliderChange) {
          imageProcessing->vignetteStretch = sliderChange.value();
        }
        sliderChange = SliderLineComponent::render(
          "Vignette FOV", imageProcessing->vignetteCameraFov, 0.f, Math::PI, 0.1f, "%.2f");
        if (sliderChange) {
          imageProcessing->vignetteCameraFov = sliderChange.value();
        }
        sliderChange = SliderLineComponent::render(
          "Vignette center X", imageProcessing->vignetteCentreX, 0.f, 1.f, 0.1f, "%.2f");
        if (sliderChange) {
          imageProcessing->vignetteCentreX = sliderChange.value();
        }
        sliderChange = SliderLineComponent::render(
          "Vignette center Y", imageProcessing->vignetteCentreY, 0.f, 1.f, 0.1f, "%.2f");
        if (sliderChange) {
          imageProcessing->vignetteCentreY = sliderChange.value();
        }
        Color3LineComponent::render("Vignette color", imageProcessing->vignetteColor);
        optionChange = OptionsLineComponent::render(
          "Vignette blend mode", imageProcessing->vignetteBlendMode(), vignetteModeOptions);
        if (optionChange) {
          imageProcessing->vignetteBlendMode = optionChange.value();
        }
      }
      imageProcessingContainerOpened = true;
    }
    else {
      imageProcessingContainerOpened = false;
    }
    // --- SHARPEN ---
    static auto sharpenContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(sharpenContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("SHARPEN")) {
      if (CheckBoxLineComponent::render("Enabled", renderPipeline->sharpenEnabled())) {
        renderPipeline->sharpenEnabled = !renderPipeline->sharpenEnabled();
      }
      if (renderPipeline->sharpenEnabled()) {
        auto sliderChange = SliderLineComponent::render(
          "Color amount", renderPipeline->sharpen->colorAmount, 0.f, 1.f, 0.05f, "%.2f");
        if (sliderChange) {
          renderPipeline->sharpen->colorAmount = sliderChange.value();
        }
        sliderChange = SliderLineComponent::render(
          "Edge amount", renderPipeline->sharpen->edgeAmount, 0.f, 5.f, 0.05f, "%.2f");
        if (sliderChange) {
          renderPipeline->sharpen->edgeAmount = sliderChange.value();
        }
      }
      sharpenContainerOpened = true;
    }
    else {
      sharpenContainerOpened = false;
    }
  }

}; // namespace BABYLON

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_POST_PROCESSES_DEFAULT_RENDERING_PIPELINE_PROPERTY_GRID_COMPONENT_H
