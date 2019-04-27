#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_TEXTURE_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_TEXTURE_PROPERTY_GRID_COMPONENT_H

#include <memory>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/text_line_component.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/materials/textures/texture_constants.h>

namespace BABYLON {

class BaseTexture;
using BaseTexturePtr = std::shared_ptr<BaseTexture>;

struct BABYLON_SHARED_EXPORT TexturePropertyGridComponent {

  static void render(const BaseTexturePtr& texture)
  {
    static std::vector<std::pair<const char*, unsigned int>> orientationOptions{
      {"Nearest", TextureConstants::NEAREST_NEAREST},
      {"Nearest & linear mip", TextureConstants::NEAREST_LINEAR},
      {"Linear", TextureConstants::LINEAR_LINEAR_MIPLINEAR}};

    // --- PREVIEW ---
    static auto previewContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(previewContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("PREVIEW")) {
      previewContainerOpened = true;
    }
    else {
      previewContainerOpened = false;
    }
    // --- GENERAL ---
    static auto generalContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(generalContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("GENERAL")) {
      TextLineComponent::render("Class", texture->getClassName());
      TextLineComponent::render("Has alpha",
                                texture->hasAlpha() ? "Yes" : "No");
      TextLineComponent::render("Is 3D", texture->is3D ? "Yes" : "No");
      TextLineComponent::render("Is cube", texture->isCube ? "Yes" : "No");
      TextLineComponent::render("Is render target",
                                texture->isRenderTarget ? "Yes" : "No");
      auto sliderChange = SliderLineComponent::render(
        "UV set", texture->coordinatesIndex, 0, 3, 1, "%.3f");
      if (sliderChange) {
        texture->coordinatesIndex
          = static_cast<unsigned int>(sliderChange.value());
      }
    }
    else {
      generalContainerOpened = false;
    }
    // --- TRANSFORM ---
    static auto transformContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(transformContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("TRANSFORM")) {
      if (!texture->isCube) {
      }
      else {
      }
      transformContainerOpened = true;
    }
    else {
      transformContainerOpened = false;
    }
  }

}; // end of struct TexturePropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MATERIALS_TEXTURE_PROPERTY_GRID_COMPONENT_H
