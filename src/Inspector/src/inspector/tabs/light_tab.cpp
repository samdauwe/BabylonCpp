#include <babylon/inspector/tabs/light_tab.h>

#include <babylon/engine/scene.h>
#include <babylon/inspector/adapters/light_adapter.h>
#include <babylon/inspector/inspector.h>
#include <babylon/inspector/treetools/abstract_tree_tool.h>
#include <babylon/lights/light.h>

#include <imgui.h>

namespace BABYLON {

LightTab::LightTab(Inspector& inspector)
    : PropertyTab("Light", inspector), _isInitialized{false}
{
  _buildTree();
}

LightTab::~LightTab()
{
}

void LightTab::_buildTree()
{
  // Get the scene object
  const auto& scene = _inspector.scene();
  if (!scene) {
    return;
  }

  // Get all lights from the first scene.
  _lights.clear();
  for (const auto& light : scene->lights) {
    _lights.emplace_back(TreeItem<LightAdapter>{
      *this, ::std::make_unique<LightAdapter>(light.get())});
  }

  // Set initialized flag
  _isInitialized = true;
}

void LightTab::_renderTree()
{
  if (!_isInitialized) {
    return;
  }

  // Lights
  for (auto& lightTreeItem : _lights) {
    // Get the item adapter
    auto& adapter = lightTreeItem.adapter();
    // Render the tools
    for (auto& tool : adapter.getTools()) {
      tool->render();
      ImGui::SameLine();
    }
    // Render light node
    if (ImGui::Selectable(adapter.id().c_str(), lightTreeItem.isActive())) {
      toggleSelection(lightTreeItem, _lights);
    }
    ImGui::SameLine();
    // Color "color-bot" #5db0d7 -> rgba(93, 176, 215, 1)
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.36f, 0.69f, 0.84f, 1.0f));
    ImGui::TextWrapped("- %s", adapter.type().c_str());
    ImGui::PopStyleColor();
  }
} // namespace BABYLON

} // end of namespace BABYLON
