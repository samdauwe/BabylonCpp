#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_LABEL_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_LABEL_COMPONENT_H

#include <functional>
#include <string>

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>
#include <babylon/core/string.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT TreeItemLabelComponent {

  static void render(const char* label, const char* icon,
                     const std::optional<ImVec4>& color = std::nullopt)
  {
    // Title icon
    if (color.has_value()) {
      ImGui::PushStyleColor(ImGuiCol_Text, *color);
    }
    ImGui::TextWrapped("%s", icon);
    if (color.has_value()) {
      ImGui::PopStyleColor();
    }
    ImGui::SameLine();
    // Title text
    ImGui::TextWrapped("%s", label);
  }

}; // end of struct TreeItemLabelComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_LABEL_COMPONENT_H
