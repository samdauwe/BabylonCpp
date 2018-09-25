#include <babylon/inspector/tabs/light_tab.h>

#include <babylon/engine/scene.h>
#include <babylon/lights/light.h>

#include <babylon/inspector/adapters/light_adapter.h>
#include <babylon/inspector/inspector.h>
#include <babylon/inspector/properties/properties_view.h>
#include <babylon/inspector/treetools/abstract_tree_tool.h>

#include <babylon/imgui/imgui_utils.h>
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
    _lights.emplace_back(
      TreeItem<LightAdapter>{*this, std::make_unique<LightAdapter>(light)});
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
    // Render tree item
    if (ImGui::Selectable(adapter.id().c_str(), lightTreeItem.isActive())) {
      toggleSelection(lightTreeItem, _lights);
    }
    ImGui::SameLine();
    // Render type information
    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorBot());
    ImGui::TextWrapped("- %s", adapter.type().c_str());
    ImGui::PopStyleColor();
  }
}

void LightTab::_renderProperties()
{
  // Lights
  for (auto& lightTreeItem : _lights) {
    if (lightTreeItem.isActive()) {
      // Get the item adapter
      auto& adapter    = lightTreeItem.adapter();
      auto& properties = adapter.getProperties();
      if (properties) {
        properties->render();
      }
    }
  }
}

} // end of namespace BABYLON
