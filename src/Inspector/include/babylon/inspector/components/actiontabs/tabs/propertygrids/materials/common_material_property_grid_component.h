#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_COMMON_MATERIAL_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_COMMON_MATERIAL_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/engines/engine_constants.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/options_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/text_line_component.h>
#include <babylon/materials/material.h>
#include <babylon/materials/pbr/pbr_base_material.h>
#include <babylon/materials/pbr/pbr_material.h>

namespace BABYLON {

class Material;
using MaterialPtr = std::shared_ptr<Material>;

struct BABYLON_SHARED_EXPORT CommonMaterialPropertyGridComponent {

  static void render(const MaterialPtr& material)
  {
    static std::vector<std::pair<const char*, unsigned int>> orientationOptions{
      {"Clockwise", Material::ClockWiseSideOrientation},
      {"Counterclockwise", Material::CounterClockWiseSideOrientation}};

    static std::vector<std::pair<const char*, unsigned int>>
      transparencyModeOptions{
        {"Opaque", PBRMaterial::PBRMATERIAL_OPAQUE},
        {"Alpha test", PBRMaterial::PBRMATERIAL_ALPHATEST},
        {"Alpha blend", PBRMaterial::PBRMATERIAL_ALPHABLEND},
        {"Alpha blend and test", PBRMaterial::PBRMATERIAL_ALPHATESTANDBLEND},
      };

    static std::vector<std::pair<const char*, unsigned int>> alphaModeOptions{
      {"Combine", EngineConstants::ALPHA_COMBINE},
      {"One one", EngineConstants::ALPHA_ONEONE},
      {"Add", EngineConstants::ALPHA_ADD},
      {"Subtract", EngineConstants::ALPHA_SUBTRACT},
      {"Multiply", EngineConstants::ALPHA_MULTIPLY},
      {"Maximized", EngineConstants::ALPHA_MAXIMIZED},
      {"Pre-multiplied", EngineConstants::ALPHA_PREMULTIPLIED},
    };

    // --- GENERAL ---
    static auto generalContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(generalContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("GENERAL")) {
      TextLineComponent::render("ID", material->id);
      TextLineComponent::render("Unique ID",
                                std::to_string(material->uniqueId));
      TextLineComponent::render("Class", material->getClassName());
      if (CheckBoxLineComponent::render("Backface culling",
                                        material->backFaceCulling())) {
        material->backFaceCulling = !material->backFaceCulling();
      }
      auto optionChange = OptionsLineComponent::render(
        "Orientation", static_cast<unsigned int>(material->sideOrientation),
        orientationOptions);
      if (optionChange) {
        material->sideOrientation = static_cast<int>(optionChange.value());
      }
      if (CheckBoxLineComponent::render("Disable depth write",
                                        material->disableDepthWrite)) {
        material->disableDepthWrite = !material->disableDepthWrite;
      }
      if (CheckBoxLineComponent::render("Need depth pre-pass",
                                        material->needDepthPrePass())) {
        material->needDepthPrePass = !material->needDepthPrePass();
      }
      if (CheckBoxLineComponent::render("Wireframe", material->wireframe())) {
        material->wireframe = !material->wireframe();
      }
      if (CheckBoxLineComponent::render("Point cloud",
                                        material->pointsCloud())) {
        material->pointsCloud = !material->pointsCloud();
      }
      auto sliderChange = SliderLineComponent::render(
        "Point size", material->pointSize, 0.f, 100.f, 0.1f, "%.2f");
      if (sliderChange) {
        material->pointSize = sliderChange.value();
      }
      sliderChange = SliderLineComponent::render("Z-offset", material->zOffset,
                                                 -10.f, 10.f, 0.1f, "%.2f");
      if (sliderChange) {
        material->zOffset = sliderChange.value();
      }
      generalContainerOpened = true;
    }
    else {
      generalContainerOpened = false;
    }
    // --- TRANSPARENCY ---
    static auto transparencyContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(transparencyContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("TRANSPARENCY")) {
      auto sliderChange = SliderLineComponent::render(
        "Alpha", material->alpha(), 0.f, 1.f, 0.01f, "%.2f");
      if (sliderChange) {
        material->alpha = sliderChange.value();
      }
      auto pbrBaseMaterial
        = std::static_pointer_cast<PBRBaseMaterial>(material);
      if (pbrBaseMaterial) {
        auto optionChange = OptionsLineComponent::render(
          "Transparency mode", *pbrBaseMaterial->transparencyMode(),
          transparencyModeOptions);
        if (optionChange) {
          pbrBaseMaterial->transparencyMode = optionChange.value();
        }
      }
      auto optionChange = OptionsLineComponent::render(
        "Alpha mode", material->alphaMode(), alphaModeOptions);
      if (optionChange) {
        material->alphaMode = optionChange.value();
      }
      if (CheckBoxLineComponent::render("Separate culling pass",
                                        material->separateCullingPass)) {
        material->separateCullingPass = !material->separateCullingPass;
      }
      transparencyContainerOpened = true;
    }
    else {
      transparencyContainerOpened = false;
    }
  }

}; // end of struct CommonMaterialPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_COMMON_MATERIAL_PROPERTY_GRID_COMPONENT_H
