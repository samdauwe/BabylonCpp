#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_SPRITES_SPRITE_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_SPRITES_SPRITE_PROPERTY_GRID_COMPONENT_H

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/actiontabs/lines/button_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/check_box_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/color3_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/float_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/slider_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/text_line_component.h>
#include <babylon/inspector/components/actiontabs/lines/vector3_line_component.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/sprites/sprite.h>
#include <babylon/sprites/sprite_manager.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT SpritePropertyGridComponent {

  static void disposeSprite(const SpritePtr& sprite)
  {
    sprite->dispose();
  }

  static void switchPlayStopState(const SpritePtr& sprite)
  {
    if (sprite->animationStarted()) {
      sprite->stopAnimation();
    }
    else {
      sprite->playAnimation(sprite->fromIndex(), sprite->toIndex(), sprite->loopAnimation(),
                            sprite->delay());
    }
  }

  static void render(const SpritePtr& sprite)
  {
    const auto manager     = sprite->manager();
    const auto textureSize = manager->texture()->getSize();
    auto maxCellCount      = 0;

    if (!textureSize.width || !textureSize.height) {
      maxCellCount = std::max(sprite->fromIndex(), sprite->toIndex());
    }
    else {
      maxCellCount
        = (textureSize.width / manager->cellWidth) * (textureSize.height / manager->cellHeight);
    }

    // --- GENERAL ---
    static auto generalContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(generalContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("GENERAL")) {
      TextLineComponent::render("Name", sprite->name);
      TextLineComponent::render("Unique ID", std::to_string(sprite->uniqueId));
      TextLineComponent::render("Link to manager", manager->name);
      if (CheckBoxLineComponent::render("Visible", sprite->isVisible)) {
        sprite->isVisible = !sprite->isVisible;
      }
      if (ButtonLineComponent::render("Dispose")) {
        disposeSprite(sprite);
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
      Vector3LineComponent::render("Position", sprite->position);
      if (CheckBoxLineComponent::render("Pickable", sprite->isPickable)) {
        sprite->isPickable = !sprite->isPickable;
      }
      if (CheckBoxLineComponent::render("Use alpha for picking", sprite->useAlphaForPicking)) {
        sprite->useAlphaForPicking = !sprite->useAlphaForPicking;
      }
      Color3LineComponent::render("Color", *sprite->color);
      const auto sliderChange
        = SliderLineComponent::render("Angle", sprite->angle, 0.f, Math::PI2, 0.01f, "%.02f");
      if (sliderChange) {
        sprite->angle = sliderChange.value();
      }
      propertiesContainerOpened = true;
    }
    else {
      propertiesContainerOpened = false;
    }
    // --- CELL ---
    static auto cellContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(cellContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("CELL")) {
      const auto sliderChange = SliderLineComponent::render("Cell index", sprite->cellIndex, 0.f,
                                                            maxCellCount, 1.f, "%.0f");
      if (sliderChange) {
        sprite->cellIndex = static_cast<int>(sliderChange.value());
      }
      if (CheckBoxLineComponent::render("Invert U axis", sprite->invertU)) {
        sprite->invertU = !sprite->invertU;
      }
      if (CheckBoxLineComponent::render("Invert V axis", sprite->invertV)) {
        sprite->invertV = !sprite->invertV;
      }
      cellContainerOpened = true;
    }
    else {
      cellContainerOpened = false;
    }
    // --- SCALE ---
    static auto scaleContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(scaleContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("SCALE")) {
      auto valueChange = FloatLineComponent::render("Width", sprite->width);
      if (valueChange && valueChange.value() >= 0.f) {
        sprite->width = valueChange.value();
      }
      valueChange = FloatLineComponent::render("height", sprite->height);
      if (valueChange && valueChange.value() >= 0.f) {
        sprite->height = valueChange.value();
      }
      scaleContainerOpened = true;
    }
    else {
      scaleContainerOpened = false;
    }
    // --- ANIMATION ---
    static auto animationContainerOpened = true;
    ImGui::SetNextTreeNodeOpen(animationContainerOpened, ImGuiCond_Always);
    if (ImGui::CollapsingHeader("ANIMATION")) {
      auto sliderChange = SliderLineComponent::render("Start cell", sprite->fromIndex, 0.f,
                                                      maxCellCount, 1.f, "%.0f");
      if (sliderChange) {
        sprite->fromIndex = static_cast<int>(sliderChange.value());
      }
      sliderChange
        = SliderLineComponent::render("End cell", sprite->toIndex, 0.f, maxCellCount, 1.f, "%.0f");
      if (sliderChange) {
        sprite->toIndex = static_cast<int>(sliderChange.value());
      }
      if (CheckBoxLineComponent::render("Loop", sprite->loopAnimation())) {
        sprite->loopAnimation = !sprite->loopAnimation();
      }
      auto valueChange = FloatLineComponent::render("Delay", sprite->delay());
      if (valueChange && valueChange.value() >= 0.f) {
        sprite->delay = static_cast<int>(valueChange.value());
      }
      if (ButtonLineComponent::render(sprite->animationStarted() ? "Stop" : "Start")) {
        switchPlayStopState(sprite);
      }
      animationContainerOpened = true;
    }
    else {
      animationContainerOpened = false;
    }
  }

}; // end of struct SpritePropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_SPRITES_SPRITE_PROPERTY_GRID_COMPONENT_H
