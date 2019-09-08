#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_PBR_MATERIAL_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_PBR_MATERIAL_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/color3_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/texture_link_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/vector2_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/materials/common_material_property_grid_component.h>
#include <babylon/materials/pbr/pbr_anisotropic_configuration.h>
#include <babylon/materials/pbr/pbr_brdf_configuration.h>
#include <babylon/materials/pbr/pbr_clear_coat_configuration.h>
#include <babylon/materials/pbr/pbr_material.h>
#include <babylon/materials/pbr/pbr_sheen_configuration.h>
#include <babylon/materials/pbr/pbr_sub_surface_configuration.h>
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
    static std::vector<std::pair<const char*, unsigned int>> debugMode{
      {"None", 0},
      // Geometry
      {"Normalized position", 1},
      {"Normals", 2},
      {"Tangents", 3},
      {"Bitangents", 4},
      {"Bump Normals", 5},
      {"UV1", 6},
      {"UV2", 7},
      {"ClearCoat Normals", 8},
      {"ClearCoat Tangents", 9},
      {"ClearCoat Bitangents", 10},
      {"Anisotropic Normals", 11},
      {"Anisotropic Tangents", 12},
      {"Anisotropic Bitangents", 13},
      // Maps
      {"Albdeo Map", 20},
      {"Ambient Map", 21},
      {"Opacity Map", 22},
      {"Emissive Map", 23},
      {"Light Map", 24},
      {"Metallic Map", 25},
      {"Reflectivity Map", 26},
      {"ClearCoat Map", 27},
      {"ClearCoat Tint Map", 28},
      {"Sheen Map", 29},
      {"Anisotropic Map", 30},
      {"Thickness Map", 31},
      // Env
      {"Env Refraction", 40},
      {"Env Reflection", 41},
      {"Env Clear Coat", 42},
      // Lighting
      {"Direct Diffuse", 50},
      {"Direct Specular", 51},
      {"Direct Clear Coat", 52},
      {"Direct Sheen", 53},
      {"Env Irradiance", 54},
      // Lighting Params
      {"Surface Albedo", 60},
      {"Reflectance 0", 61},
      {"Roughness", 62},
      {"AlphaG", 63},
      {"NdotV", 64},
      {"ClearCoat Color", 65},
      {"ClearCoat Roughness", 66},
      {"ClearCoat NdotV", 67},
      {"Transmittance", 68},
      {"Refraction Transmittance", 69},
      // Misc
      {"SEO", 70},
      {"EHO", 71},
      {"Energy Factor", 72},
      {"Specular Reflectance", 73},
      {"Clear Coat Reflectance", 74},
      {"Sheen Reflectance", 75},
      {"Luminance Over Alpha", 76},
      {"Alpha", 77},
    };

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
      auto sliderChange = SliderLineComponent::render(
        "Micro-surface", material->microSurface(), 0.f, 1.f, 0.01f, "%.2f");
      if (sliderChange) {
        material->microSurface = sliderChange.value();
      }
      Color3LineComponent::render(
        "Emissive", material->emissiveColor(),
        [&material](const Color3& color) { material->emissiveColor = color; });
      Color3LineComponent::render(
        "Ambient", material->ambientColor(),
        [&material](const Color3& color) { material->ambientColor = color; });
      if (CheckBoxLineComponent::render("Use physical light falloff",
                                        material->usePhysicalLightFalloff())) {
        material->usePhysicalLightFalloff
          = !material->usePhysicalLightFalloff();
      }
      lightingAndColorsContainerOpened = true;
    }
    else {
      lightingAndColorsContainerOpened = false;
    }
    // --- METALLIC WORKFLOW ---
    static auto metallicWorkflowOpened = false;
    ImGui::SetNextTreeNodeOpen(metallicWorkflowOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("METALLIC WORKFLOW")) {
      auto sliderChange = SliderLineComponent::render(
        "Metallic", material->metallic().value_or(0.f), 0.f, 1.f, 0.01f,
        "%.2f");
      if (sliderChange) {
        material->metallic = sliderChange.value();
      }
      sliderChange = SliderLineComponent::render(
        "Roughness", material->roughness().value_or(0.f), 0.f, 1.f, 0.01f,
        "%.2f");
      if (sliderChange) {
        material->roughness = sliderChange.value();
      }
      metallicWorkflowOpened = true;
    }
    else {
      metallicWorkflowOpened = false;
    }
    // --- CLEAR COAT ---
    static auto clearCoatOpened = false;
    ImGui::SetNextTreeNodeOpen(clearCoatOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("CLEAR COAT")) {
      auto& clearCoat = material->clearCoat;
      if (CheckBoxLineComponent::render("Enabled", clearCoat->isEnabled())) {
        clearCoat->isEnabled = !clearCoat->isEnabled();
      }
      if (clearCoat->isEnabled()) {
        // Fragment
        auto sliderChange = SliderLineComponent::render(
          "Intensity", clearCoat->intensity, 0.f, 1.f, 0.01f, "%.2f");
        if (sliderChange) {
          clearCoat->intensity = sliderChange.value();
        }
        sliderChange = SliderLineComponent::render(
          "Roughness", clearCoat->roughness, 0.f, 1.f, 0.01f, "%.2f");
        if (sliderChange) {
          clearCoat->roughness = sliderChange.value();
        }
        sliderChange = SliderLineComponent::render(
          "IOR", clearCoat->indiceOfRefraction(), 0.f, 1.f, 0.01f, "%.2f");
        if (sliderChange) {
          clearCoat->indiceOfRefraction = sliderChange.value();
        }
        TextureLinkLineComponent::render("Texture", material,
                                         clearCoat->texture());
        TextureLinkLineComponent::render("Bump", material,
                                         clearCoat->bumpTexture());
        if (clearCoat->bumpTexture()) {
          sliderChange = SliderLineComponent::render(
            "Bump strength", clearCoat->bumpTexture()->level, 0.f, 2.f, 0.01f,
            "%.2f");
          if (sliderChange) {
            clearCoat->bumpTexture()->level = sliderChange.value();
          }
        }
        if (CheckBoxLineComponent::render("Tint", clearCoat->isTintEnabled())) {
          clearCoat->isTintEnabled = !clearCoat->isTintEnabled();
        }
        if (clearCoat->isTintEnabled()) {
          Color3LineComponent::render("Tint Color", clearCoat->tintColor);
          sliderChange = SliderLineComponent::render(
            "At Distance", clearCoat->tintColorAtDistance, 0.f, 20.f, 0.1f,
            "%.1f");
          if (sliderChange) {
            clearCoat->tintColorAtDistance = sliderChange.value();
          }
          sliderChange = SliderLineComponent::render("Tint Thickness",
                                                     clearCoat->tintThickness,
                                                     0.f, 20.f, 0.1f, "%.1f");
          if (sliderChange) {
            clearCoat->tintThickness = sliderChange.value();
          }
          TextureLinkLineComponent::render("Tint Texture", material,
                                           clearCoat->tintTexture());
        }
      }
      clearCoatOpened = true;
    }
    else {
      clearCoatOpened = false;
    }
    // --- ANISOTROPIC ---
    static auto anisotropicOpened = false;
    ImGui::SetNextTreeNodeOpen(anisotropicOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("ANISOTROPIC")) {
      auto& anisotropy = material->anisotropy;
      if (CheckBoxLineComponent::render("Enabled", anisotropy->isEnabled())) {
        anisotropy->isEnabled = !anisotropy->isEnabled();
      }
      if (anisotropy->isEnabled()) {
        // Fragment
        auto sliderChange = SliderLineComponent::render(
          "Intensity", anisotropy->intensity, 0.f, 1.f, 0.01f, "%.2f");
        if (sliderChange) {
          anisotropy->intensity = sliderChange.value();
        }
        Vector2LineComponent::render("Direction", anisotropy->direction);
        TextureLinkLineComponent::render("Texture", material,
                                         anisotropy->texture());
      }
      anisotropicOpened = true;
    }
    else {
      anisotropicOpened = false;
    }
    // --- SHEEN ---
    static auto sheenOpened = false;
    ImGui::SetNextTreeNodeOpen(sheenOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("SHEEN")) {
      auto& sheen = material->sheen;
      if (CheckBoxLineComponent::render("Enabled", sheen->isEnabled())) {
        sheen->isEnabled = !sheen->isEnabled();
      }
      if (sheen->isEnabled()) {
        // Fragment
        if (CheckBoxLineComponent::render("Link to Albedo",
                                          sheen->linkSheenWithAlbedo())) {
          sheen->linkSheenWithAlbedo = !sheen->linkSheenWithAlbedo();
        }
        auto sliderChange = SliderLineComponent::render(
          "Intensity", sheen->intensity, 0.f, 1.f, 0.01f, "%.2f");
        if (sliderChange) {
          sheen->intensity = sliderChange.value();
        }
        Color3LineComponent::render("Color", sheen->color);
        TextureLinkLineComponent::render("Texture", material, sheen->texture());
      }
      sheenOpened = true;
    }
    else {
      sheenOpened = false;
    }
    // --- SUBSURFACE ---
    static auto subsurfaceOpened = false;
    ImGui::SetNextTreeNodeOpen(subsurfaceOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("SUBSURFACE")) {
      auto& subSurface = material->subSurface;
      TextureLinkLineComponent::render("Thickness", material,
                                       subSurface->thicknessTexture());
      auto sliderChange = SliderLineComponent::render(
        "Min Thickness", subSurface->minimumThickness, 0.f, 10.f, 0.1f, "%.1f");
      if (sliderChange) {
        subSurface->minimumThickness = sliderChange.value();
      }
      sliderChange = SliderLineComponent::render(
        "Max Thickness", subSurface->maximumThickness, 0.f, 10.f, 0.1f, "%.1f");
      if (sliderChange) {
        subSurface->maximumThickness = sliderChange.value();
      }
      if (CheckBoxLineComponent::render(
            "Mask From Thickness", subSurface->useMaskFromThicknessTexture())) {
        subSurface->useMaskFromThicknessTexture
          = !subSurface->useMaskFromThicknessTexture();
      }
      Color3LineComponent::render("Tint Color", subSurface->tintColor);
      if (CheckBoxLineComponent::render("Refraction Enabled",
                                        subSurface->isRefractionEnabled())) {
        subSurface->isRefractionEnabled = !subSurface->isRefractionEnabled();
      }
      if (subSurface->isRefractionEnabled()) {
        // Fragment
        sliderChange = SliderLineComponent::render(
          "Intensity", subSurface->refractionIntensity, 0.f, 1.f, 0.01f,
          "%.2f");
        if (sliderChange) {
          subSurface->refractionIntensity = sliderChange.value();
        }
        sliderChange = SliderLineComponent::render(
          "Index of Refraction", subSurface->indexOfRefraction, 1.f, 2.f, 0.01f,
          "%.2f");
        if (sliderChange) {
          subSurface->indexOfRefraction = sliderChange.value();
        }
        sliderChange = SliderLineComponent::render(
          "Tint at Distance", subSurface->tintColorAtDistance, 1.f, 10.f, 0.1f,
          "%.1f");
        if (sliderChange) {
          subSurface->tintColorAtDistance = sliderChange.value();
        }
        if (CheckBoxLineComponent::render(
              "Link refraction with transparency",
              subSurface->linkRefractionWithTransparency())) {
          subSurface->linkRefractionWithTransparency
            = !subSurface->linkRefractionWithTransparency();
        }
      }
      Color3LineComponent::render("Tint Color", subSurface->tintColor);
      if (CheckBoxLineComponent::render("Transluency Enabled",
                                        subSurface->isTranslucencyEnabled())) {
        subSurface->isTranslucencyEnabled
          = !subSurface->isTranslucencyEnabled();
      }
      if (subSurface->isTranslucencyEnabled()) {
        sliderChange = SliderLineComponent::render(
          "Intensity", subSurface->translucencyIntensity, 0.f, 1.f, 0.01f,
          "%.2f");
        if (sliderChange) {
          subSurface->translucencyIntensity = sliderChange.value();
        }
        Color3LineComponent::render("Diffusion Distance",
                                    subSurface->diffusionDistance);
      }
      subsurfaceOpened = true;
    }
    else {
      subsurfaceOpened = false;
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
        "Specular", material->specularIntensity(), 0.f, 1.f, 0.01f, "%.2f");
      if (sliderChange) {
        material->specularIntensity = sliderChange.value();
      }
      sliderChange = SliderLineComponent::render(
        "Emissive", material->emissiveIntensity(), 0.f, 1.f, 0.01f, "%.2f");
      if (sliderChange) {
        material->emissiveIntensity = sliderChange.value();
      }
      sliderChange = SliderLineComponent::render(
        "Direct", material->directIntensity(), 0.f, 1.f, 0.01f, "%.2f");
      if (sliderChange) {
        material->directIntensity = sliderChange.value();
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
      if (CheckBoxLineComponent::render(
            "Energy Conservation", material->brdf->useEnergyConservation())) {
        material->brdf->useEnergyConservation
          = !material->brdf->useEnergyConservation();
      }
      if (CheckBoxLineComponent::render(
            "Spherical Harmonics", material->brdf->useSphericalHarmonics())) {
        material->brdf->useSphericalHarmonics
          = !material->brdf->useSphericalHarmonics();
      }
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
    // --- Debug ---
    static auto debugOpened = false;
    ImGui::SetNextTreeNodeOpen(debugOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("DEBUG")) {
      auto optionChange = OptionsLineComponent::render(
        "Debug mode", material->debugMode(), debugMode);
      if (optionChange) {
        material->debugMode = optionChange.value();
      }
      auto sliderChange = SliderLineComponent::render(
        "Split position", material->debugLimit, -1.f, 1.f, 0.01f, "%.2f");
      if (sliderChange) {
        material->debugLimit = sliderChange.value();
      }
      sliderChange = SliderLineComponent::render(
        "Output factor", material->debugFactor, 0.f, 5.f, 0.01f, "%.2f");
      if (sliderChange) {
        material->debugFactor = sliderChange.value();
      }
      debugOpened = true;
    }
    else {
      debugOpened = false;
    }
  }

}; // end of struct PBRMaterialPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_PBR_MATERIAL_PROPERTY_GRID_COMPONENT_H
