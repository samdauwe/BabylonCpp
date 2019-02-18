#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_STANDARD_MATERIAL_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_STANDARD_MATERIAL_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/color3_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/texture_link_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/materials/common_material_property_grid_component.h>
#include <babylon/materials/background/background_material.h>

namespace BABYLON {

class BackgroundMaterial;
using BackgroundMaterialPtr = std::shared_ptr<BackgroundMaterial>;

struct BABYLON_SHARED_EXPORT StandardMaterialPropertyGridComponent {

  static void renderTextures(const BackgroundMaterialPtr& material)
  {
    // --- TEXTURES ---
    static auto texturesContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(texturesContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("TEXTURES")) {
      TextureLinkLineComponent::render("Diffuse", material,
                                       material->diffuseTexture());
      TextureLinkLineComponent::render("Reflection", material,
                                       material->reflectionTexture());
      if (material->reflectionTexture()) {
        SliderLineComponent::render("Reflection blur", material->reflectionBlur,
                                    0.f, 1.f, 0.01f, "%.2f");
      }
      texturesContainerOpened = true;
    }
    else {
      texturesContainerOpened = false;
    }
  }

  static void render(const BackgroundMaterialPtr& material)
  {
    CommonMaterialPropertyGridComponent::render(material);
    // --- LIGHTING & COLORS ---
    static auto lightingAndColorsContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(lightingAndColorsContainerOpened,
                               ImGuiCond_Always);
    if (ImGui::CollapsingHeader("LIGHTING & COLORS")) {
      Color3LineComponent::render(
        "Primary", material->primaryColor(),
        [&material](const Color3& color) { material->primaryColor = color; });
      SliderLineComponent::render(
        "Shadow level", material->primaryColorShadowLevel(), 0.f, 1.f, 0.01f,
        [&material](float value) { material->primaryColorShadowLevel = value; },
        "%.2f");
      SliderLineComponent::render(
        "Highlight level", material->primaryColorHighlightLevel(), 0.f, 1.f,
        0.01f,
        [&material](float value) {
          material->primaryColorHighlightLevel = value;
        },
        "%.2f");
      lightingAndColorsContainerOpened = true;
    }
    else {
      lightingAndColorsContainerOpened = false;
    }
    // --- RENDERING ---
    static auto renderingContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(renderingContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("RENDERING")) {
      CheckBoxLineComponent::render(
        "Enable noise", material->enableNoise(),
        [&material](bool value) { material->enableNoise = value; });
      CheckBoxLineComponent::render(
        "Opacity fresnel", material->opacityFresnel(),
        [&material](bool value) { material->opacityFresnel = value; });
      CheckBoxLineComponent::render(
        "Reflection fresnel", material->reflectionFresnel(),
        [&material](bool value) { material->reflectionFresnel = value; });
      SliderLineComponent::render(
        "Reflection amount", material->reflectionAmount(), 0.f, 1.f, 0.01f,
        [&material](float value) { material->reflectionAmount = value; },
        "%.2f");
      renderingContainerOpened = true;
    }
    else {
      renderingContainerOpened = false;
    }
  }

}; // end of struct StandardMaterialPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_STANDARD_MATERIAL_PROPERTY_GRID_COMPONENT_H
