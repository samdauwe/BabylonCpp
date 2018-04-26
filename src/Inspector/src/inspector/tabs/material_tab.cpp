#include <babylon/inspector/tabs/material_tab.h>

#include <babylon/engine/scene.h>
#include <babylon/inspector/adapters/material_adapter.h>
#include <babylon/inspector/inspector.h>
#include <babylon/inspector/treetools/abstract_tree_tool.h>
#include <babylon/materials/material.h>

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

  // Get all lights from the first scene.
  _materials.clear();
  for (const auto& mat : scene->materials) {
    _materials.emplace_back(TreeItem<MaterialAdapter>{
      *this, ::std::make_unique<MaterialAdapter>(mat.get())});
  }

  // Set initialized flag
  _isInitialized = true;
}

void MaterialTab::_renderTree()
{
  if (!_isInitialized) {
    return;
  }

  // Lights
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
    // Color "color-bot" #5db0d7 -> rgba(93, 176, 215, 1)
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.36f, 0.69f, 0.84f, 1.0f));
    ImGui::TextWrapped("- %s", adapter.type().c_str());
    ImGui::PopStyleColor();
  }
}

} // end of namespace BABYLON
