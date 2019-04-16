#include <babylon/inspector/components/sceneexplorer/scene_graph_component.h>

#include <imgui.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/node.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/light.h>
#include <babylon/materials/material.h>
#include <babylon/mesh/mesh.h>

#include <babylon/inspector/components/sceneexplorer/entities/camera_tree_item_component.h>
#include <babylon/inspector/components/sceneexplorer/entities/group_container_tree_item.h>
#include <babylon/inspector/components/sceneexplorer/entities/light_tree_item_component.h>
#include <babylon/inspector/components/sceneexplorer/entities/material_tree_item_component.h>
#include <babylon/inspector/components/sceneexplorer/entities/mesh_tree_item_component.h>
#include <babylon/inspector/components/sceneexplorer/entities/scene_tree_item_component.h>

#include <babylon/inspector/components/sceneexplorer/tree_item_expandable_header_component.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_root_header_component.h>

namespace BABYLON {

SceneGraphComponent::SceneGraphComponent(Scene* scene)
    : _scene{scene}
    , _sceneGraph{createSceneTreeItem(scene)}
    , _initialized{false}
{
  _treeItemComparator = [](const TreeItem& a, const TreeItem& b) -> bool {
    return a->labelWithoutIcon < b->labelWithoutIcon;
  };
}

SceneGraphComponent::SceneGraphComponent::~SceneGraphComponent()
{
}

void SceneGraphComponent::reinitialize()
{
  // Nodes
  const auto& rootNodes = _scene->rootNodes;
  auto& nodesTreeItem
    = _sceneGraph.root().addChild(createGroupContainer("Nodes"));
  for (auto&& rootNode : rootNodes) {
    _initializeNodesTreeItem(nodesTreeItem, rootNode->getAsNodePtr());
  }
  // Materials
  const auto& materials = _scene->materials;
  auto& materialsTreeItem
    = _sceneGraph.root().addChild(createGroupContainer("Materials"));
  for (auto&& material : materials) {
    _initializeTreeItem(materialsTreeItem, material);
  }
  // Textures
  _sceneGraph.root().addChild(createGroupContainer("Textures"));
  // Rendering pipelines
  _sceneGraph.root().addChild(createGroupContainer("Rendering pipelines"));

  _initialized = true;
}

void SceneGraphComponent::DeSelectAll(TreeNode<TreeItem>& node)
{
  {
    Tree<TreeItem>::recursive_visit(node, [](TreeNode<TreeItem>& TteeItem) {
      TteeItem.data()->isActive = false;
    });
  }
}

void SceneGraphComponent::_initializeNodesTreeItem(
  TreeNode<TreeItem>& parentTreeItem, const NodePtr& node)
{
  // Create TreeItem
  auto& treeItem = parentTreeItem.addChildSorted(createNodeTreeItem(node),
                                                 _treeItemComparator);

  // Create Children
  for (const auto& child : node->getChildren()) {
    if (child) {
      _initializeNodesTreeItem(treeItem, child);
    }
  }
}

TreeItem SceneGraphComponent::createGroupContainer(const char* label)
{
  return std::make_shared<GroupContainerTreeItemComponent>(label);
}

TreeItem
SceneGraphComponent::createMaterialTreeItem(const MaterialPtr& material)
{
  if (material) {
    IMaterialTreeItemComponentProps props;
    props.material = material;
    return std::make_shared<MaterialTreeItemComponent>(props);
  }

  return nullptr;
}

TreeItem SceneGraphComponent::createNodeTreeItem(const NodePtr& node)
{
  if (node) {
    if (String::contains(node->getClassName(), "Bone")) {
    }
    else if (String::contains(node->getClassName(), "Camera")) {
      ICameraTreeItemComponentProps props;
      props.camera = std::static_pointer_cast<Camera>(node);
      return std::make_shared<CameraTreeItemComponent>(props);
    }
    else if (String::contains(node->getClassName(), "Light")) {
      ILightTreeItemComponentProps props;
      props.light = std::static_pointer_cast<Light>(node);
      return std::make_shared<LightTreeItemComponent>(props);
    }
    else if (String::contains(node->getClassName(), "Mesh")) {
      auto mesh = std::static_pointer_cast<Mesh>(node);
      if (mesh->getTotalVertices() > 0) {
        IMeshTreeItemComponentProps props;
        props.mesh = std::static_pointer_cast<Mesh>(node);
        return std::make_shared<MeshTreeItemComponent>(props);
      }
    }
  }

  return nullptr;
}

TreeItem SceneGraphComponent::createSceneTreeItem(Scene* scene)
{
  if (scene) {
    ISceneTreeItemComponentProps props;
    props.scene = scene;
    return std::make_shared<SceneTreeItemComponent>(props);
  }

  return nullptr;
}

void SceneGraphComponent::_renderTree(TreeNode<TreeItem>& node)
{
  if (!_initialized) {
    reinitialize();
  }

  ImGuiTreeNodeFlags node_flags
    = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
  if (node.isLeaf()) {
    if (node.depth() == 1) { // Render group containers
      node_flags |= ImGuiTreeNodeFlags_Leaf;
      if (ImGui::TreeNodeEx(node.data()->label, node_flags)) {
        if (ImGui::IsItemClicked()) {
        }
        ImGui::TreePop();
      }
    }
    else {
      auto& nodeData = node.data();
      if (nodeData) {
        ImGui::PushID(nodeData->labelWithoutIcon.c_str());
        if (ImGui::Selectable("", nodeData->isActive,
                              ImGuiSelectableFlags_SpanAllColumns)) {
          if (!nodeData->isActive) {
            DeSelectAll(_sceneGraph.root());
            nodeData->isActive = !nodeData->isActive;
            std::cout << nodeData->label << std::endl;
          }
        }
        ImGui::PopID();
        ImGui::SameLine();
        nodeData->renderLabelWithIcon();
        ImGui::NextColumn();
        nodeData->renderControls();
        ImGui::NextColumn();
      }
      /*else { // Render tree item
        ImGui::Text(">");
        ImGui::SameLine();
        if (ImGui::Selectable(nodeData->label, nodeData->isActive)) {
          if (!nodeData->isActive) {
            DeSelectAll(_sceneGraph.root());
            nodeData->isActive = !nodeData->isActive;
            std::cout << nodeData->label << std::endl;
          }
        }
        ImGui::NextColumn();

        // ImGui::Button(nodeData.name);
        ImGui::NextColumn();
      }*/
    }
  }
  else {
    ImGui::AlignFirstTextHeightToWidgets(); // not needed to solve bug. Helps
                                            // make vertical spacing more
                                            // consistent
    if (node.isRoot()) {
      node_flags |= (node.data()->isActive ? ImGuiTreeNodeFlags_Selected : 0);
    }
    if (ImGui::TreeNodeEx(node.data()->label, node_flags)) {
      if (ImGui::IsItemClicked() && node.isRoot() && !node.data()->isActive) {
        DeSelectAll(_sceneGraph.root());
        auto& nodeData     = node.data();
        nodeData->isActive = !nodeData->isActive;
      }
      ImGui::NextColumn();
      node.data()->renderControls();
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
#if 0
  ImGui::Columns(2, "SceneGraphComponent", false);
  ImGui::SetNextTreeNodeOpen(true);
  ImGui::Unindent();
  _renderTree(_sceneGraph.root());
  ImGui::Indent();
  ImGui::Columns(1);

  ImGui::Columns(2, "mycolumns"); // 4-ways, with border
  ImGui::Separator();
  ImGui::Text("ID");
  ImGui::NextColumn();
  ImGui::Text("Name");
  ImGui::NextColumn();
  ImGui::Separator();
  const char* names[3] = {"One", "Two", "Three"};
  static int selected  = -1;
  for (int i = 0; i < 3; i++) {
    char label[32];
    sprintf(label, "%04d", i);
    if (ImGui::Selectable(label, selected == i,
                          ImGuiSelectableFlags_SpanAllColumns)) {
      selected = i;
    }
    ImGui::NextColumn();
    if (selected == 0 && i == 0)
      ImGui::Text(" v %s", names[i]);
    else
      ImGui::Text("> %s", names[i]);
    ImGui::NextColumn();

    if (selected == 0 && i == 0) {
      ImGui::Text("---> %s", names[i]);
      ImGui::NextColumn();
      ImGui::Text("***> %s", names[i]);
      if (ImGui::IsItemClicked(0)) {
          std::cout << "Clicked" << std::endl;
        names[i] = "###";
      }
      ImGui::NextColumn();
    }
  }
  ImGui::Columns(1);
#else
  TreeItemExpandableHeaderComponent({false, "Nodes"}).render();
  TreeItemExpandableHeaderComponent({false, "Materials"}).render();
  TreeItemRootHeaderComponent::render("Textures");
  TreeItemRootHeaderComponent::render("Rendering pipelines");
#endif
}

} // namespace BABYLON
