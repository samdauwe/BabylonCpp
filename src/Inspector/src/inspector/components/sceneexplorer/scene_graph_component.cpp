#include <babylon/inspector/components/sceneexplorer/scene_graph_component.h>

#include <imgui.h>

#include <babylon/engine/node.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/material.h>

namespace BABYLON {

SceneGraphComponent::SceneGraphComponent(Scene* scene)
    : _scene{scene}, _sceneGraph{TreeItem{"Scene"}}, _initialized{false}
{
}

SceneGraphComponent::SceneGraphComponent::~SceneGraphComponent()
{
}

void SceneGraphComponent::reinitialize()
{
  // Nodes
  const auto& rootNodes = _scene->rootNodes;
  auto& nodesTreeItem   = _sceneGraph.root().addChild(TreeItem{"Nodes"});
  for (auto&& rootNode : rootNodes) {
    _initializeNodesTreeItem(nodesTreeItem, rootNode);
  }
  // Materials
  const auto& materials   = _scene->materials;
  auto& materialsTreeItem = _sceneGraph.root().addChild(TreeItem{"Materials"});
  for (auto&& material : materials) {
    _initializeTreeItem(materialsTreeItem, material);
  }

  _initialized = true;
}

void SceneGraphComponent::DeSelectAll(TreeNode<TreeItem>& node)
{
  {
    Tree<TreeItem>::recursive_visit(node, [](TreeNode<TreeItem>& TteeItem) {
      TteeItem.data().isActive = false;
    });
  }
}

void SceneGraphComponent::_renderTree(TreeNode<TreeItem>& node)
{
  if (!_initialized) {
    reinitialize();
  }

  ImGuiTreeNodeFlags node_flags
    = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
  if (node.isLeaf()) {
    if (node.depth() == 1) {
      node_flags |= ImGuiTreeNodeFlags_Leaf;
      if (ImGui::TreeNodeEx(node.data().label, node_flags)) {
        if (ImGui::IsItemClicked()) {
        }
        ImGui::TreePop();
      }
    }
    else {
      // Render tree item
      // ImGui::Text(">");
      // ImGui::SameLine();
      auto& nodeData = node.data();
      if (ImGui::Selectable(nodeData.label, nodeData.isActive)) {
        if (!nodeData.isActive) {
          DeSelectAll(_sceneGraph.root());
          nodeData.isActive = !nodeData.isActive;
          std::cout << nodeData.label << std::endl;
        }
      }
      ImGui::NextColumn();

      // ImGui::Button(nodeData.name);
      ImGui::NextColumn();
    }
  }
  else {
    ImGui::AlignFirstTextHeightToWidgets(); // not needed to solve bug. Helps
                                            // make vertical spacing more
                                            // consistent
    if (node.isRoot()) {
      node_flags |= (node.data().isActive ? ImGuiTreeNodeFlags_Selected : 0);
    }
    if (ImGui::TreeNodeEx(node.data().label, node_flags)) {
      if (ImGui::IsItemClicked() && node.isRoot() && !node.data().isActive) {
        DeSelectAll(_sceneGraph.root());
        auto& nodeData    = node.data();
        nodeData.isActive = !nodeData.isActive;
      }
      // ADDED THESE TWO LINES
      ImGui::NextColumn();
      ImGui::NextColumn();

      for (const auto& child : node.children()) {
        _renderTree(*child);
      }
      ImGui::TreePop();
    }
  }
}

void SceneGraphComponent::render()
{
  ImGui::Columns(2, "SceneGraphComponent", false);
  ImGui::SetNextTreeNodeOpen(true);
  ImGui::Unindent();
  _renderTree(_sceneGraph.root());
  ImGui::Indent();
  ImGui::Columns(1);
}

} // namespace BABYLON
