#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_PBR_MATERIAL_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_PBR_MATERIAL_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/color3_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/texture_link_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/materials/common_material_property_grid_component.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/textures/base_texture.h>

namespace BABYLON {

class PBRMaterial;
using PBRMaterialPtr = std::shared_ptr<PBRMaterial>;

struct BABYLON_SHARED_EXPORT PBRMaterialPropertyGridComponent {

  static void renderTextures(const PBRMaterialPtr& material)
  {
    if (material->getActiveTextures().empty()) {
      return;
    }

    // --- TEXTURES ---
    static auto texturesContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(texturesContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("TEXTURES")) {
      TextureLinkLineComponent::render("Albedo", material,
                                       material->albedoTexture());
      TextureLinkLineComponent::render("Metallic", material,
                                       material->metallicTexture());
      TextureLinkLineComponent::render("Reflection", material,
                                       material->reflectionTexture());
      TextureLinkLineComponent::render("Refraction", material,
                                       material->refractionTexture());
      TextureLinkLineComponent::render("Micro-surface", material,
                                       material->microSurfaceTexture());
      TextureLinkLineComponent::render("Bump", material,
                                       material->bumpTexture());
      TextureLinkLineComponent::render("Emissive", material,
                                       material->emissiveTexture());
      TextureLinkLineComponent::render("Opacity", material,
                                       material->opacityTexture());
      TextureLinkLineComponent::render("Ambient", material,
                                       material->ambientTexture());
      TextureLinkLineComponent::render("Lightmap", material,
                                       material->lightmapTexture());
      texturesContainerOpened = true;
    }
    else {
      texturesContainerOpened = false;
    }
  }

  static void render(const PBRMaterialPtr& material)
  {
    CommonMaterialPropertyGridComponent::render(material);
    renderTextures(material);
    // --- LIGHTING & COLORS ---
    static auto lightingAndColorsContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(lightingAndColorsContainerOpened,
                               ImGuiCond_Always);
    if (ImGui::CollapsingHeader("LIGHTING & COLORS")) {
      Color3LineComponent::render(
        "Albedo", material->albedoColor(),
        [&material](const Color3& color) { material->albedoColor = color; });
      Color3LineComponent::render("Reflectivity", material->reflectivityColor(),
                                  [&material](const Color3& color) {
                                    material->reflectivityColor = color;
                                  });
      Color3LineComponent::render(
        "Emissive", material->emissiveColor(),
        [&material](const Color3& color) { material->emissiveColor = color; });
      Color3LineComponent::render(
        "Ambient", material->ambientColor(),
        [&material](const Color3& color) { material->ambientColor = color; });
      lightingAndColorsContainerOpened = true;
    }
    else {
      lightingAndColorsContainerOpened = false;
    }
    // --- LEVELS ---
    static auto levelsContainerOpened = false;
    ImGui::SetNextTreeNodeOpen(levelsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("LEVELS")) {
      auto sliderChange = SliderLineComponent::render(
        "Environment", material->environmentIntensity(), 0.f, 1.f, 0.01f,
        "%.2f");
      if (sliderChange) {
        material->environmentIntensity = sliderChange.value();
      }
      sliderChange = SliderLineComponent::render(
        "Metallic", *material->metallic(), 0.f, 1.f, 0.01f, "%.2f");
      if (sliderChange) {
        material->metallic = sliderChange.value();
      }
      sliderChange = SliderLineComponent::render(
        "Roughness", *material->roughness(), 0.f, 1.f, 0.01f, "%.2f");
      if (sliderChange) {
        material->roughness = sliderChange.value();
      }
      sliderChange = SliderLineComponent::render(
        "Micro-surface", material->microSurface(), 0.f, 1.f, 0.01f, "%.2f");
      if (sliderChange) {
        material->microSurface = sliderChange.value();
      }
      sliderChange = SliderLineComponent::render(
        "Specular", material->specularIntensity(), 0.f, 1.f, 0.01f, "%.2f");
      if (sliderChange) {
        material->specularIntensity = sliderChange.value();
      }
      if (material->bumpTexture()) {
        sliderChange = SliderLineComponent::render(
          "Bump strength", material->bumpTexture()->level, 0.f, 1.f, 0.01f,
          "%.2f");
        if (sliderChange) {
          material->bumpTexture()->level = sliderChange.value();
        }
      }
      if (material->ambientTexture()) {
        sliderChange = SliderLineComponent::render(
          "Ambient strength", material->ambientTexture()->level, 0.f, 1.f,
          0.01f, "%.2f");
        if (sliderChange) {
          material->ambientTexture()->level = sliderChange.value();
        }
      }
      if (material->reflectionTexture()) {
        sliderChange = SliderLineComponent::render(
          "Reflection strength", material->reflectionTexture()->level, 0.f, 1.f,
          0.01f, "%.2f");
        if (sliderChange) {
          material->reflectionTexture()->level = sliderChange.value();
        }
      }
      levelsContainerOpened = true;
    }
    else {
      levelsContainerOpened = false;
    }
    // --- RENDERING ---
    static auto renderingContainerOpened = false;
    ImGui::SetNextTreeNodeOpen(renderingContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("RENDERING")) {
      if (CheckBoxLineComponent::render(
            "Alpha from albedo", material->useAlphaFromAlbedoTexture())) {
        material->useAlphaFromAlbedoTexture
          = !material->useAlphaFromAlbedoTexture();
      }
      if (CheckBoxLineComponent::render("Ambient in grayscale",
                                        material->useAmbientInGrayScale())) {
        material->useAmbientInGrayScale = !material->useAmbientInGrayScale();
      }
      if (CheckBoxLineComponent::render("Radiance over alpha",
                                        material->useRadianceOverAlpha())) {
        material->useRadianceOverAlpha = !material->useRadianceOverAlpha();
      }
      if (CheckBoxLineComponent::render(
            "Link refraction with transparency",
            material->linkRefractionWithTransparency())) {
        material->linkRefractionWithTransparency
          = !material->linkRefractionWithTransparency();
      }
      if (CheckBoxLineComponent::render(
            "Micro-surface from ref. map alpha",
            material->useMicroSurfaceFromReflectivityMapAlpha())) {
        material->useMicroSurfaceFromReflectivityMapAlpha
          = !material->useMicroSurfaceFromReflectivityMapAlpha();
      }
      if (CheckBoxLineComponent::render("Specular over alpha",
                                        material->useSpecularOverAlpha())) {
        material->useSpecularOverAlpha = !material->useSpecularOverAlpha();
      }
      if (CheckBoxLineComponent::render(
            "Specular anti-aliasing", material->enableSpecularAntiAliasing())) {
        material->enableSpecularAntiAliasing
          = !material->enableSpecularAntiAliasing();
      }
      renderingContainerOpened = true;
    }
    else {
      renderingContainerOpened = false;
    }
    // --- ADVANCED ---
    static auto advancedContainerOpened = false;
    ImGui::SetNextTreeNodeOpen(advancedContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("ADVANCED")) {
      if (CheckBoxLineComponent::render("Radiance occlusion",
                                        material->useRadianceOcclusion())) {
        material->useRadianceOcclusion = !material->useRadianceOcclusion();
      }
      if (CheckBoxLineComponent::render("Horizon occlusion",
                                        material->useHorizonOcclusion())) {
        material->useHorizonOcclusion = !material->useHorizonOcclusion();
      }
      if (CheckBoxLineComponent::render("Unlit", material->unlit())) {
        material->unlit = !material->unlit();
      }
      advancedContainerOpened = true;
    }
    else {
      advancedContainerOpened = false;
    }
  }

}; // end of struct PBRMaterialPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_PBR_MATERIAL_PROPERTY_GRID_COMPONENT_H
