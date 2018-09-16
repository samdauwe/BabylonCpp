#include <babylon/inspector/tabs/camera_tab.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/scene.h>

#include <babylon/inspector/adapters/camera_adapter.h>
#include <babylon/inspector/inspector.h>
#include <babylon/inspector/properties/properties_view.h>
#include <babylon/inspector/treetools/abstract_tree_tool.h>

#include <babylon/imgui/imgui_utils.h>
#include <imgui.h>

namespace BABYLON {

CameraTab::CameraTab(Inspector& inspector)
    : PropertyTab("Camera", inspector), _isInitialized{false}
{
  _buildTree();
}

CameraTab::~CameraTab()
{
}

void CameraTab::_buildTree()
{
  // Get the scene object
  const auto& scene = _inspector.scene();
  if (!scene) {
    return;
  }

  // Get all cameras from the first scene.
  _cameras.clear();
  for (const auto& camera : scene->cameras) {
    _cameras.emplace_back(TreeItem<CameraAdapter>{
      *this, ::std::make_unique<CameraAdapter>(camera)});
  }

  // Set initialized flag
  _isInitialized = true;
}

void CameraTab::_renderTree()
{
  if (!_isInitialized) {
    return;
  }

  // Cameras
  for (auto& cameraTreeItem : _cameras) {
    // Get the item adapter
    auto& adapter = cameraTreeItem.adapter();
    // Render the tools
    for (auto& tool : adapter.getTools()) {
      tool->render();
      ImGui::SameLine();
    }
    // Render tree item
    if (ImGui::Selectable(adapter.id().c_str(), cameraTreeItem.isActive())) {
      toggleSelection(cameraTreeItem, _cameras);
    }
    ImGui::SameLine();
    // Render type information
    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorBot());
    ImGui::TextWrapped("- %s", adapter.type().c_str());
    ImGui::PopStyleColor();
  }
}

void CameraTab::_renderProperties()
{
  // Cameras
  for (auto& cameraTreeItem : _cameras) {
    if (cameraTreeItem.isActive()) {
      // Get the item adapter
      auto& adapter    = cameraTreeItem.adapter();
      auto& properties = adapter.getProperties();
      if (properties) {
        properties->render();
      }
    }
  }
}

} // end of namespace BABYLON
