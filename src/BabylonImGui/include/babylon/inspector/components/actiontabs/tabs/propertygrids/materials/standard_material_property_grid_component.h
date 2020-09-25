#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_STANDARD_MATERIAL_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_STANDARD_MATERIAL_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/color3_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/texture_link_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/materials/common_material_property_grid_component.h>
#include <babylon/materials/detail_map_configuration.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/base_texture.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT StandardMaterialPropertyGridComponent {

  static void renderTextures(const StandardMaterialPtr& material)
  {
    if (material->getActiveTextures().empty()) {
      return;
    }

    // --- TEXTURES ---
    static auto texturesContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(texturesContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("TEXTURES")) {
      TextureLinkLineComponent::render("Diffuse", material, material->diffuseTexture());
      TextureLinkLineComponent::render("Specular", material, material->specularTexture());
      TextureLinkLineComponent::render("Reflection", material, material->reflectionTexture());
      TextureLinkLineComponent::render("Refraction", material, material->refractionTexture());
      TextureLinkLineComponent::render("Emissive", material, material->emissiveTexture());
      TextureLinkLineComponent::render("Bump", material, material->bumpTexture());
      TextureLinkLineComponent::render("Opacity", material, material->opacityTexture());
      TextureLinkLineComponent::render("Ambient", material, material->ambientTexture());
      TextureLinkLineComponent::render("Lightmap", material, material->lightmapTexture());
      TextureLinkLineComponent::render("Detailmap", material, material->detailMap->texture());
      if (CheckBoxLineComponent::render("Use lightmap as shadowmap",
                                        material->useLightmapAsShadowmap())) {
        material->useLightmapAsShadowmap = !material->useLightmapAsShadowmap();
      }
      if (CheckBoxLineComponent::render("Use detailmap", material->detailMap->isEnabled())) {
        material->detailMap->isEnabled = !material->detailMap->isEnabled();
      }
      texturesContainerOpened = true;
    }
    else {
      texturesContainerOpened = false;
    }
  }

  static void render(const StandardMaterialPtr& material)
  {
    CommonMaterialPropertyGridComponent::render(material);
    renderTextures(material);
    // --- LIGHTING & COLORS ---
    static auto lightingAndColorsOpened = true;
    ImGui::SetNextTreeNodeOpen(lightingAndColorsOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("LIGHTING & COLORS")) {
      Color3LineComponent::render("Diffuse", material->diffuseColor);
      Color3LineComponent::render("Specular", material->specularColor);
      auto sliderChange = SliderLineComponent::render("Specular power", material->specularPower,
                                                      0.f, 128.f, 0.1f, "%.2f");
      if (sliderChange) {
        material->specularPower = sliderChange.value();
      }
      Color3LineComponent::render("Emissive", material->emissiveColor);
      Color3LineComponent::render("Ambient", material->ambientColor);
      if (CheckBoxLineComponent::render("Use specular over alpha",
                                        material->useSpecularOverAlpha())) {
        material->useSpecularOverAlpha = !material->useSpecularOverAlpha();
      }
      lightingAndColorsOpened = true;
    }
    else {
      lightingAndColorsOpened = false;
    }
    // --- LEVELS ---
    static auto levelsOpened = false;
    ImGui::SetNextTreeNodeOpen(levelsOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("LEVELS")) {
      if (material->diffuseTexture()) {
        auto sliderChange = SliderLineComponent::render(
          "Diffuse level", material->diffuseTexture()->level, 0.f, 2.f, 0.01f, "%.2f");
        if (sliderChange) {
          material->diffuseTexture()->level = sliderChange.value();
        }
      }
      if (material->specularTexture()) {
        auto sliderChange = SliderLineComponent::render(
          "Specular level", material->specularTexture()->level, 0.f, 2.f, 0.01f, "%.2f");
        if (sliderChange) {
          material->specularTexture()->level = sliderChange.value();
        }
      }
      if (material->reflectionTexture()) {
        auto sliderChange = SliderLineComponent::render(
          "Reflection level", material->reflectionTexture()->level, 0.f, 2.f, 0.01f, "%.2f");
        if (sliderChange) {
          material->reflectionTexture()->level = sliderChange.value();
        }
      }
      if (material->refractionTexture()) {
        auto sliderChange = SliderLineComponent::render(
          "Refraction level", material->refractionTexture()->level, 0.f, 2.f, 0.01f, "%.2f");
        if (sliderChange) {
          material->refractionTexture()->level = sliderChange.value();
        }
      }
      if (material->emissiveTexture()) {
        auto sliderChange = SliderLineComponent::render(
          "Emissive level", material->emissiveTexture()->level, 0.f, 2.f, 0.01f, "%.2f");
        if (sliderChange) {
          material->emissiveTexture()->level = sliderChange.value();
        }
      }
      if (material->bumpTexture()) {
        auto sliderChange = SliderLineComponent::render(
          "Bump level", material->bumpTexture()->level, 0.f, 2.f, 0.01f, "%.2f");
        if (sliderChange) {
          material->bumpTexture()->level = sliderChange.value();
        }
      }
      if (material->opacityTexture()) {
        auto sliderChange = SliderLineComponent::render(
          "Opacity level", material->opacityTexture()->level, 0.f, 2.f, 0.01f, "%.2f");
        if (sliderChange) {
          material->opacityTexture()->level = sliderChange.value();
        }
      }
      if (material->ambientTexture()) {
        auto sliderChange = SliderLineComponent::render(
          "Ambient level", material->ambientTexture()->level, 0.f, 2.f, 0.01f, "%.2f");
        if (sliderChange) {
          material->ambientTexture()->level = sliderChange.value();
        }
      }
      if (material->lightmapTexture()) {
        auto sliderChange = SliderLineComponent::render(
          "Lightmap level", material->lightmapTexture()->level, 0.f, 2.f, 0.01f, "%.2f");
        if (sliderChange) {
          material->lightmapTexture()->level = sliderChange.value();
        }
      }
      if (material->detailMap->isEnabled()) {
        auto sliderChange = SliderLineComponent::render(
          "Detailmap diffuse", material->detailMap->diffuseBlendLevel, 0.f, 1.f, 0.01f, "%.2f");
        if (sliderChange) {
          material->detailMap->diffuseBlendLevel = sliderChange.value();
        }
        sliderChange = SliderLineComponent::render("Detailmap bump", material->detailMap->bumpLevel,
                                                   0.f, 1.f, 0.01f, "%.2f");
        if (sliderChange) {
          material->detailMap->bumpLevel = sliderChange.value();
        }
      }
      levelsOpened = true;
    }
    else {
      levelsOpened = false;
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

}; // end of struct StandardMaterialPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_STANDARD_MATERIAL_PROPERTY_GRID_COMPONENT_H
