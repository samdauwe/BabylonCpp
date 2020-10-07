#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_PBR_SPECULAR_GLOSSINESS_MATERIAL_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_PBR_SPECULAR_GLOSSINESS_MATERIAL_PROPERTY_GRID_COMPONENT_H

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/lines/color3_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/texture_link_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/materials/common_material_property_grid_component.h>
#include <babylon/materials/pbr/pbr_specular_glossiness_material.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT PBRSpecularGlossinessMaterialPropertyGridComponent {

  static void renderTextures(const PBRSpecularGlossinessMaterialPtr& material)
  {
    if (material->getActiveTextures().empty()) {
      return;
    }

    // --- TEXTURES ---
    static auto texturesContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(texturesContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("TEXTURES")) {
      TextureLinkLineComponent::render("Diffuse", material, material->diffuseTexture());
      TextureLinkLineComponent::render("Specular glossiness", material,
                                       material->specularGlossinessTexture());
      TextureLinkLineComponent::render("Normal", material, material->normalTexture());
      TextureLinkLineComponent::render("Environment", material, material->environmentTexture());
      TextureLinkLineComponent::render("Emissive", material, material->emissiveTexture());
      TextureLinkLineComponent::render("Lightmap", material, material->lightmapTexture());
      texturesContainerOpened = true;
    }
    else {
      texturesContainerOpened = false;
    }
  }

  static void render(const PBRSpecularGlossinessMaterialPtr& material)
  {
    CommonMaterialPropertyGridComponent::render(material);
    renderTextures(material);
    // --- LIGHTING & COLORS ---
    static auto lightingAndColorsContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(lightingAndColorsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("LIGHTING & COLORS")) {
      Color3LineComponent::render("Diffuse", material->diffuseColor());
      Color3LineComponent::render("Specular", material->specularColor());
      lightingAndColorsContainerOpened = true;
    }
    else {
      lightingAndColorsContainerOpened = false;
    }
    // --- LEVELS ---
    static auto levelsContainerOpened = false;
    ImGui::SetNextTreeNodeOpen(levelsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("LEVELS")) {
      auto sliderChange = SliderLineComponent::render("Glossiness", material->glossiness(), 0.f,
                                                      1.f, 0.01f, "%.2f");
      if (sliderChange) {
        material->glossiness = sliderChange.value();
      }
      levelsContainerOpened = true;
    }
    else {
      levelsContainerOpened = false;
    }
    // --- NORMAL MAP ---
    static auto normalMapOpened = false;
    ImGui::SetNextTreeNodeOpen(normalMapOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("NORMAL MAP")) {
      if (CheckBoxLineComponent::render("Invert X axis", material->invertNormalMapX())) {
        material->invertNormalMapX = !material->invertNormalMapX();
      }
      if (CheckBoxLineComponent::render("Invert Y axis", material->invertNormalMapY())) {
        material->invertNormalMapY = !material->invertNormalMapY();
      }
      normalMapOpened = true;
    }
    else {
      normalMapOpened = false;
    }
  }

}; // end of struct PBRSpecularGlossinessMaterialPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_PBR_SPECULAR_GLOSSINESS_MATERIAL_PROPERTY_GRID_COMPONENT_H
