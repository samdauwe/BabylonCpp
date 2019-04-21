#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_ICON_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_ICON_COMPONENT_H

// ImGui
#include <babylon/imgui/imgui_utils.h>

#include <babylon/babylon_api.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT TreeItemIconComponent {

  static bool render(const char* icon, const char* tooltip, float offsetX,
                     bool isActive = true)
  {
    ImGui::SameLine(offsetX);
    ImGui::PushStyleColor(
      ImGuiCol_Text,
      ImVec4(1.0f, 1.0f, 1.0f, isActive ? 1.f : ImGui::IsNotActiveOpacity));
    ImGui::TextWrapped("%s", icon);
    ImGui::PopStyleColor();
    if (ImGui::IsItemHovered()) {
      ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
      ImGui::SetTooltip("%s", tooltip);
    }
    return ImGui::IsItemClicked(0);
  }

}; // end of struct TreeItemIconComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_ICON_COMPONENT_H
