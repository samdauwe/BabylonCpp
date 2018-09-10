#include <babylon/inspector/tabs/mesh_tab.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engine/node.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/abstract_mesh.h>

#include <babylon/inspector/adapters/mesh_adapter.h>
#include <babylon/inspector/helpers/helpers.h>
#include <babylon/inspector/inspector.h>
#include <babylon/inspector/properties/properties_view.h>
#include <babylon/inspector/treetools/abstract_tree_tool.h>

#include <babylon/imgui/imgui_utils.h>
#include <imgui.h>

namespace BABYLON {

MeshTab::MeshTab(Inspector& inspector)
    : PropertyTab("Mesh", inspector), _isInitialized{false}
{
  _buildTree();
}

MeshTab::~MeshTab()
{
}

void MeshTab::_buildTree()
{
  // Create meshes alias
  auto& arr = _meshes;
  // Tab containing mesh already in results
  vector_t<NodePtr> alreadyIn;

  // Recursive method building the tree panel
  ::std::function<TreeItem<MeshAdapter>(NodePtr)> createNode
    = [&](const NodePtr& obj) -> TreeItem<MeshAdapter> {
    auto descendants = obj->getDescendants(true);

    auto node
      = TreeItem<MeshAdapter>{*this, ::std::make_unique<MeshAdapter>(obj)};

    if (!descendants.empty()) {
      for (auto& child : descendants) {
        if (child->type() == IReflect::Type::TRANSFORMNODE) {
          if (!Helpers::IsSystemName(child->name)) {
            node.children.emplace_back(createNode(child));
          }
        }
      }
      node.update();
    }

    // Retrieve the root node if the mesh is actually child of another mesh
    // This can hapen if the child mesh has been created before the parent mesh
    if (obj->parent() != nullptr && stl_util::contains(alreadyIn, obj)) {
      size_t i      = 0;
      bool notFound = true;
      // Find and delete the root node standing for this mesh
      while (i < arr.size() && notFound) {
        if (obj->name == arr[i].id()) {
          stl_util::remove(arr, i);
          notFound = false;
        }
        ++i;
      }
    }

    alreadyIn.emplace_back(obj);
    return node;
  };

  // Get the scene object
  const auto& scene = _inspector.scene();
  if (!scene) {
    return;
  }

  // Find top of hierarchy for meshes...
  vector_t<NodePtr> meshWithoutAnyParent;
  for (auto& mesh : scene->meshes) {
    // Not already in the array, not system name and no parent
    auto _mesh = std::dynamic_pointer_cast<Node>(mesh);
    if (!stl_util::contains(meshWithoutAnyParent, _mesh)
        && !Helpers::IsSystemName(_mesh->name) && !_mesh->parent) {
      meshWithoutAnyParent.emplace_back(_mesh);
    }
  }
  // ... and for transforms
  for (auto& tn : scene->transformNodes) {
    // Not already in the array, not system name and no parent
    auto _tn = std::dynamic_pointer_cast<Node>(tn);
    if (!stl_util::contains(meshWithoutAnyParent, _tn)
        && !Helpers::IsSystemName(_tn->name) && !_tn->parent) {
      meshWithoutAnyParent.emplace_back(_tn);
    }
  }

  for (auto& mesh : meshWithoutAnyParent) {
    if (!stl_util::contains(alreadyIn, mesh)
        && !Helpers::IsSystemName(mesh->name)) {
      arr.emplace_back(createNode(mesh));
    }
  }

  // Set initialized flag
  _isInitialized = true;
}

void MeshTab::_renderTree()
{
  if (!_isInitialized) {
    return;
  }

  // Meshes
  for (auto& meshTreeItem : _meshes) {
    // Get the item adapter
    auto& adapter = meshTreeItem.adapter();
    // Render the tools
    for (auto& tool : adapter.getTools()) {
      tool->render();
      ImGui::SameLine();
    }
    // Render tree item
    if (ImGui::Selectable(adapter.id().c_str(), meshTreeItem.isActive())) {
      toggleSelection(meshTreeItem, _meshes);
    }
    ImGui::SameLine();
    // Render type information
    ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetColorBot());
    ImGui::TextWrapped("- %s", adapter.type().c_str());
    ImGui::PopStyleColor();
  }
}

void MeshTab::_renderProperties()
{
  // Meshes
  for (auto& meshTreeItem : _meshes) {
    if (meshTreeItem.isActive()) {
      // Get the item adapter
      auto& adapter    = meshTreeItem.adapter();
      auto& properties = adapter.getProperties();
      if (properties) {
        properties->render();
      }
    }
  }
}

} // end of namespace BABYLON
