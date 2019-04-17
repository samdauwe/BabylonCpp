#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_EXPANDABLE_HEADER_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_EXPANDABLE_HEADER_COMPONENT_H

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>
#include <babylon/imgui/icons_font_awesome_5.h>
#include <babylon/imgui/imgui_utils.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT TreeItemExpandableHeaderComponent {

  /** Font Awesome Icon Unicodes **/
  static constexpr const char* faMinus           = ICON_FA_MINUS;
  static constexpr const char* faPlus            = ICON_FA_PLUS;
  static constexpr const char* faCompress        = ICON_FA_COMPRESS;
  static constexpr const char* faExpandArrowsAlt = ICON_FA_EXPAND_ARROWS_ALT;

  static void render(bool& isExpanded, const char* label, bool& mustExpand)
  {
    // Arrow icon
    ImGui::TextWrapped("%s", isExpanded ? faMinus : faPlus);
    if (ImGui::IsItemClicked(0)) {
      // Switch expanded state
      isExpanded = !isExpanded;
      mustExpand = false;
    }
    ImGui::SameLine();
    // Text value
    ImGui::TextWrapped("%s", label);
    ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - ImGui::IconSizeHalf);
    // ExpandAll icon
    ImGui::TextWrapped("%s", isExpanded ? faCompress : faExpandArrowsAlt);
    if (ImGui::IsItemClicked(0)) {
      // Expand all
      isExpanded = !isExpanded;
      mustExpand = !isExpanded;
    }
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("%s", isExpanded ? "Collapse all" : "Expand all");
    }
  }

}; // end of struct TreeItemExpandableHeaderComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_EXPANDABLE_HEADER_COMPONENT_H
