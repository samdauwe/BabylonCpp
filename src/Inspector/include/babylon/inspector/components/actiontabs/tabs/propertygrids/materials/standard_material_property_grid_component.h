#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_STANDARD_MATERIAL_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_STANDARD_MATERIAL_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/lines/color3_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/texture_link_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/materials/common_material_property_grid_component.h>
#include <babylon/materials/standard_material.h>
#include <babylon/materials/textures/base_texture.h>

namespace BABYLON {

class StandardMaterial;
using StandardMaterialPtr = std::shared_ptr<StandardMaterial>;

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
      TextureLinkLineComponent::render("Diffuse", material,
                                       material->diffuseTexture());
      TextureLinkLineComponent::render("Specular", material,
                                       material->specularTexture());
      TextureLinkLineComponent::render("Reflection", material,
                                       material->reflectionTexture());
      TextureLinkLineComponent::render("Refraction", material,
                                       material->refractionTexture());
      TextureLinkLineComponent::render("Emissive", material,
                                       material->emissiveTexture());
      TextureLinkLineComponent::render("Bump", material,
                                       material->bumpTexture());
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
      SliderLineComponent::render("Specular power", material->specularPower,
                                  0.f, 128.f, 0.1f, "%.2f");
      Color3LineComponent::render("Emissive", material->emissiveColor);
      Color3LineComponent::render("Ambient", material->ambientColor);
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
        SliderLineComponent::render("Diffuse level",
                                    material->diffuseTexture()->level, 0.f, 2.f,
                                    0.01f, "%.2f");
      }
      if (material->specularTexture()) {
        SliderLineComponent::render("Specular level",
                                    material->specularTexture()->level, 0.f,
                                    2.f, 0.01f, "%.2f");
      }
      if (material->reflectionTexture()) {
        SliderLineComponent::render("Reflection level",
                                    material->reflectionTexture()->level, 0.f,
                                    2.f, 0.01f, "%.2f");
      }
      if (material->refractionTexture()) {
        SliderLineComponent::render("Refraction level",
                                    material->refractionTexture()->level, 0.f,
                                    2.f, 0.01f, "%.2f");
      }
      if (material->emissiveTexture()) {
        SliderLineComponent::render("Emissive level",
                                    material->emissiveTexture()->level, 0.f,
                                    2.f, 0.01f, "%.2f");
      }
      if (material->bumpTexture()) {
        SliderLineComponent::render("Bump level",
                                    material->bumpTexture()->level, 0.f, 2.f,
                                    0.01f, "%.2f");
      }
      if (material->opacityTexture()) {
        SliderLineComponent::render("Opacity level",
                                    material->opacityTexture()->level, 0.f, 2.f,
                                    0.01f, "%.2f");
      }
      if (material->ambientTexture()) {
        SliderLineComponent::render("Ambient level",
                                    material->ambientTexture()->level, 0.f, 2.f,
                                    0.01f, "%.2f");
      }
      if (material->lightmapTexture()) {
        SliderLineComponent::render("Lightmap level",
                                    material->lightmapTexture()->level, 0.f,
                                    2.f, 0.01f, "%.2f");
      }
      levelsOpened = true;
    }
    else {
      levelsOpened = false;
    }
  }

}; // end of struct StandardMaterialPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_STANDARD_MATERIAL_PROPERTY_GRID_COMPONENT_H
