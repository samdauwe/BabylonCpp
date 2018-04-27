#include <babylon/inspector/treetools/debug_area.h>

#include <babylon/babylon_stl.h>
#include <babylon/inspector/treetools/itool_debug.h>

#include <imgui.h>

namespace BABYLON {

DebugArea::DebugArea(IToolDebug* obj) : AbstractTreeTool{}
{
  _obj = obj;
}

DebugArea::~DebugArea()
{
}

DebugArea::DebugArea(DebugArea&& other)
    : AbstractTreeTool{std::move(other)}, _obj{::std::move(other._obj)}
{
}

void DebugArea::render()
{
  // Render visibility icon (eye / eye-slash)
  if (_on) {
    // Default color
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.26f, 0.59f, 0.98f, 0.40f));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4(0.26f, 0.59f, 0.98f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          ImVec4(0.06f, 0.53f, 0.98f, 1.00f));
  }
  else {
    // Black color
    ImGui::PushStyleColor(ImGuiCol_Button,
                          static_cast<ImVec4>(ImColor::HSV(0.0f, 0.0f, 0.0f)));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                          ImVec4(0.26f, 0.59f, 0.98f, 1.00f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                          static_cast<ImVec4>(ImColor::HSV(0.0f, 0.0f, 0.0f)));
  }
  if (ImGui::Button(_on ? VISIBLE_ICON : INVISIBLE_ICON)) {
    action();
  }
  ImGui::PopStyleColor(3);
}

void DebugArea::action()
{
  AbstractTreeTool::action();
  _obj->debug(_on);
}

} // end of namespace BABYLON
