#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_EXPANDABLE_HEADER_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_EXPANDABLE_HEADER_COMPONENT_H

#include <functional>

// ImGui
#include <imgui.h>

#include <babylon/babylon_api.h>
#include <babylon/imgui/icons_font_awesome_5.h>

namespace BABYLON {

struct ITreeItemExpandableHeaderComponentProps {
  bool isExpanded                              = false;
  const char* label                            = nullptr;
  std::function<void()> onClick                = nullptr;
  std::function<void(bool expand)> onExpandAll = nullptr;
}; // end of ITreeItemExpandableHeaderComponentProps

class BABYLON_SHARED_EXPORT TreeItemExpandableHeaderComponent {

public:
  /** Font Awesome Icon Unicodes **/
  static constexpr const char* faMinus           = ICON_FA_MINUS;
  static constexpr const char* faPlus            = ICON_FA_PLUS;
  static constexpr const char* faCompress        = ICON_FA_COMPRESS;
  static constexpr const char* faExpandArrowsAlt = ICON_FA_EXPAND_ARROWS_ALT;

public:
  TreeItemExpandableHeaderComponent(
    const ITreeItemExpandableHeaderComponentProps& iProps)
      : props{iProps}
  {
  }
  ~TreeItemExpandableHeaderComponent()
  {
  }

public:
  void expandAll()
  {
    if (props.onExpandAll) {
      props.onExpandAll(!props.isExpanded);
    }
  }

  void render()
  {
    // Arrow icon
    ImGui::TextWrapped("%s", props.isExpanded ? faMinus : faPlus);
    if (ImGui::IsItemClicked(0) && props.onClick) {
      props.onClick();
    }
    ImGui::SameLine();
    // Text value
    ImGui::TextWrapped("%s", props.label);
    ImGui::SameLine();
    // ExpandAll icon
    ImGui::TextWrapped("%s", props.isExpanded ? faCompress : faExpandArrowsAlt);
    if (ImGui::IsItemClicked(0)) {
      expandAll();
    }
    if (ImGui::IsItemHovered()) {
      ImGui::SetTooltip("%s", props.isExpanded ? "Collapse all" : "Expand all");
    }
  }

private:
  ITreeItemExpandableHeaderComponentProps props;

}; // end of class TreeItemExpandableHeaderComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_TREE_ITEM_EXPANDABLE_HEADER_COMPONENT_H
