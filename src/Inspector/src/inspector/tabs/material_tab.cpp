#include <babylon/inspector/tabs/material_tab.h>

#include <babylon/engine/scene.h>
#include <babylon/materials/material.h>

#include <babylon/inspector/adapters/material_adapter.h>
#include <babylon/inspector/inspector.h>
#include <babylon/inspector/properties/properties_view.h>
#include <babylon/inspector/treetools/abstract_tree_tool.h>

#include <babylon/imgui/imgui_utils.h>
#include <imgui.h>

namespace BABYLON {

MaterialTab::MaterialTab(Inspector& inspector)
    : PropertyTab("Material", inspector), _isInitialized{false}
{
  _buildTree();
}

MaterialTab::~MaterialTab()
{
}

void MaterialTab::_buildTree()
{
  // Get the scene object
  const auto& scene = _inspector.scene();
  if (!scene) {
    return;
  }

  // Get all materials from the first scene.
  _materials.clear();
  for (const auto& mat : scene->materials) {
    _materials.emplace_back(TreeItem<MaterialAdapter>{
      *this, ::std::make_unique<MaterialAdapter>(mat)});
  }

  // Set initialized flag
  _isInitialized = true;
}

void MaterialTab::_renderTree()
{
  if (!_isInitialized
      || _inspector.scene()->materials.size() != _materials.size()) {
    _buildTree();
  }

  // Materials
  for (auto& materialTreeItem : _materials) {
    // Get the item adapter
    auto& adapter = materialTreeItem.adapter();
    // Render the tools
    for (auto& tool : adapter.getTools()) {
      tool->render();
      ImGui::SameLine();
    }
    // Render tree item
    if (ImGui::Selectable(adapter.id().c_str(), materialTreeItem.isActive())) {
      toggleSelection(materialTreeItem, _materials);
    }
    ImGui::SameLine();
    // Render type information
    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorBot());
    ImGui::TextWrapped("- %s", adapter.type().c_str());
    ImGui::PopStyleColor();
  }
}

void MaterialTab::_renderProperties()
{
  // Materials
  for (auto& materialTreeItem : _materials) {
    if (materialTreeItem.isActive()) {
      // Get the item adapter
      auto& adapter    = materialTreeItem.adapter();
      auto& properties = adapter.getProperties();
      if (properties) {
        properties->render();
      }
    }
  }
}

} // end of namespace BABYLON
