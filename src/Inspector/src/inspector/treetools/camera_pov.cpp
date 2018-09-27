#include <babylon/inspector/treetools/camera_pov.h>

#include <utility>

#include <babylon/inspector/treetools/icamera_pov.h>

#include <imgui.h>

namespace BABYLON {

CameraPOV::CameraPOV(ICameraPOV* camera) : AbstractTreeTool{}
{
  _cameraPOV = camera;
  // Put the right icon
  _on = (_cameraPOV->cameraId() == _cameraPOV->getCurrentActiveCamera());
}

CameraPOV::~CameraPOV()
{
}

CameraPOV::CameraPOV(CameraPOV&& other)
    : AbstractTreeTool{std::move(other)}
    , _cameraPOV{std::move(other._cameraPOV)}
{
}

void CameraPOV::render()
{
  // Render radio button icon (circle / check-circle)
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
  if (ImGui::Button(_on ? POV_ACTIVE_ICON : POV_INACTIVE_ICON)) {
    action();
  }
  ImGui::PopStyleColor(3);
}

void CameraPOV::action()
{
  AbstractTreeTool::action();
  _on = (_cameraPOV->cameraId() == _cameraPOV->getCurrentActiveCamera());
  _gotoPOV();
}

void CameraPOV::_gotoPOV()
{
  // Setting the point off view to the right camera
  _cameraPOV->setPOV();
}

} // end of namespace BABYLON
