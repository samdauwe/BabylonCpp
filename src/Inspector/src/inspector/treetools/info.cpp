#include <babylon/inspector/treetools/info.h>

#include <babylon/babylon_stl.h>
#include <babylon/inspector/treetools/itool_info.h>

#include <imgui.h>

namespace BABYLON {

Info::Info(IToolInfo* obj) : AbstractTreeTool{}
{
  _obj = obj;
}

Info::~Info()
{
}

Info::Info(Info&& other)
    : AbstractTreeTool{std::move(other)}, _obj{std::move(other._obj)}
{
}

void Info::render()
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
  if (ImGui::IsItemHovered()) {
    ImGui::SetTooltip("%s", _obj->getInfo().c_str());
  }
  ImGui::PopStyleColor(3);
}

void Info::action()
{
  AbstractTreeTool::action();
}

} // end of namespace BABYLON
