#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_SPRITES_SPRITE_MANAGER_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_SPRITES_SPRITE_MANAGER_PROPERTY_GRID_COMPONENT_H

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/lines/button_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/float_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/options_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/text_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/texture_link_line_component.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/rendering/rendering_manager.h>
#include <babylon/sprites/sprite.h>
#include <babylon/sprites/sprite_manager.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT SpriteManagerPropertyGridComponent {

  static void addNewSprite(const SpriteManagerPtr& spriteManager)
  {
    Sprite::New("new sprite", spriteManager);
  }

  static void disposeManager(const SpriteManagerPtr& spriteManager)
  {
    spriteManager->dispose();
  }

  static void render(const SpriteManagerPtr& spriteManager)
  {
    static const std::vector<std::pair<const char*, unsigned int>> alphaModeOptions{
      {"Combine", Constants::ALPHA_COMBINE},
      {"One one", Constants::ALPHA_ONEONE},
      {"Add", Constants::ALPHA_ADD},
      {"Subtract", Constants::ALPHA_SUBTRACT},
      {"Multiply", Constants::ALPHA_MULTIPLY},
      {"Maximized", Constants::ALPHA_MAXIMIZED},
      {"Pre-multiplied", Constants::ALPHA_PREMULTIPLIED},
    };

    // --- GENERAL ---
    static auto generalContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(generalContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("GENERAL")) {
      TextLineComponent::render("Name", spriteManager->name);
      TextLineComponent::render("Unique ID", std::to_string(spriteManager->uniqueId));
      TextLineComponent::render("Capacity", std::to_string(spriteManager->capacity()));
      TextureLinkLineComponent::render("Texture", spriteManager->texture());
      if (spriteManager->sprites.size() < spriteManager->capacity()) {
        if (ButtonLineComponent::render("Add new sprite")) {
          addNewSprite(spriteManager);
        }
      }
      if (ButtonLineComponent::render("Dispose")) {
        disposeManager(spriteManager);
      }
      generalContainerOpened = true;
    }
    else {
      generalContainerOpened = false;
    }
    // --- PROPERTIES ---
    static auto propertiesContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(propertiesContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("PROPERTIES")) {
      if (CheckBoxLineComponent::render("Pickable", spriteManager->isPickable)) {
        spriteManager->isPickable = !spriteManager->isPickable;
      }
      if (CheckBoxLineComponent::render("Fog enabled", spriteManager->fogEnabled)) {
        spriteManager->fogEnabled = !spriteManager->fogEnabled;
      }
      if (CheckBoxLineComponent::render("No depth write", spriteManager->disableDepthWrite)) {
        spriteManager->disableDepthWrite = !spriteManager->disableDepthWrite;
      }
      const auto sliderChange
        = SliderLineComponent::render("Rendering group ID", spriteManager->renderingGroupId,
                                      RenderingManager::MIN_RENDERINGGROUPS,
                                      RenderingManager::MAX_RENDERINGGROUPS - 1, 1.f, "%.0f");
      if (sliderChange) {
        spriteManager->renderingGroupId = static_cast<unsigned>(sliderChange.value());
      }
      const auto optionChange
        = OptionsLineComponent::render("Alpha mode", spriteManager->blendMode(), alphaModeOptions);
      if (optionChange) {
        spriteManager->blendMode = optionChange.value();
      }
      propertiesContainerOpened = true;
    }
    else {
      propertiesContainerOpened = false;
    }
    // --- CELLS ---
    static auto cellsContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(cellsContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("CELLS")) {
      auto valueChange = FloatLineComponent::render("Cell width", spriteManager->cellWidth);
      if (valueChange) {
        const auto value = valueChange.value();
        if (value >= 0.f) {
          spriteManager->cellWidth = static_cast<int>(valueChange.value());
        }
      }
      valueChange = FloatLineComponent::render("Cell height", spriteManager->cellHeight);
      if (valueChange) {
        const auto value = valueChange.value();
        if (value >= 0.f) {
          spriteManager->cellHeight = static_cast<int>(valueChange.value());
        }
      }
      cellsContainerOpened = true;
    }
    else {
      cellsContainerOpened = false;
    }
  }

}; // end of struct SpriteManagerPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_SPRITES_SPRITE_MANAGER_PROPERTY_GRID_COMPONENT_H
