#include <babylon/inspector/tabs/camera_tab.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/scene.h>
#include <babylon/inspector/adapters/camera_adapter.h>
#include <babylon/inspector/inspector.h>
#include <babylon/inspector/treetools/abstract_tree_tool.h>

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
      *this, ::std::make_unique<CameraAdapter>(camera.get())});
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
    // Color "color-bot" #5db0d7 -> rgba(93, 176, 215, 1)
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.36f, 0.69f, 0.84f, 1.0f));
    ImGui::TextWrapped("- %s", adapter.type().c_str());
    ImGui::PopStyleColor();
  }
}

} // end of namespace BABYLON
