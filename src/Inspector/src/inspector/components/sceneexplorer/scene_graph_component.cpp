#include <babylon/inspector/components/sceneexplorer/scene_graph_component.h>

#include <imgui.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/node.h>
#include <babylon/engine/scene.h>
#include <babylon/lights/light.h>
#include <babylon/materials/material.h>
#include <babylon/mesh/mesh.h>

#include <babylon/imgui/imgui_utils.h>

#include <babylon/inspector/components/global_state.h>
#include <babylon/inspector/components/sceneexplorer/entities/camera_tree_item_component.h>
#include <babylon/inspector/components/sceneexplorer/entities/light_tree_item_component.h>
#include <babylon/inspector/components/sceneexplorer/entities/material_tree_item_component.h>
#include <babylon/inspector/components/sceneexplorer/entities/mesh_tree_item_component.h>
#include <babylon/inspector/components/sceneexplorer/entities/scene_tree_item_component.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_expandable_header_component.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_root_header_component.h>

namespace BABYLON {

SceneGraphComponent::SceneGraphComponent(const SceneGraphComponentProps& iProps)
    : props{iProps}
    , _sceneGraph{_createSceneTreeItem(iProps.scene)}
    , _initialized{false}
{
  _treeItemComparator = [](const TreeItem& a, const TreeItem& b) -> bool {
    const auto labelA = std::string(a.label);
    const auto labelB = std::string(b.label);
    return labelA < labelB;
  };
}

SceneGraphComponent::SceneGraphComponent::~SceneGraphComponent()
{
}

void SceneGraphComponent::reinitialize()
{
  // Nodes
  const auto& rootNodes = props.scene->rootNodes;
  auto& nodesTreeItem   = _sceneGraph.root().addChild(_createTreeItem("Nodes"));
  for (const auto& rootNode : rootNodes) {
    _initializeNodesTreeItem(nodesTreeItem, rootNode->getAsNodePtr());
  }
  // Materials
  const auto& materials = props.scene->materials;
  auto& materialsTreeItem
    = _sceneGraph.root().addChild(_createTreeItem("Materials"));
  for (const auto& material : materials) {
    _initializeTreeItem(materialsTreeItem, material);
  }
  // Textures
  _sceneGraph.root().addChild(_createTreeItem("Textures"));
  // Rendering pipelines
  _sceneGraph.root().addChild(_createTreeItem("Rendering pipelines"));

  // Set the offset
  Tree<TreeItem>::recursive_visit(
    _sceneGraph.root(), [this](TreeNode<TreeItem>& node) {
      node.data().offset = _calculateOffset(node.depth());
    });

  _initialized = true;
}

void SceneGraphComponent::DeSelectAll(TreeNode<TreeItem>& node)
{
  {
    Tree<TreeItem>::recursive_visit(
      node, [](TreeNode<TreeItem>& node) { node.data().isSelected = false; });
  }
}

void SceneGraphComponent::_initializeNodesTreeItem(
  TreeNode<TreeItem>& parentTreeItem, const NodePtr& node)
{
  // Create TreeItem
  auto& treeItem = parentTreeItem.addChildSorted(_createNodeTreeItem(node),
                                                 _treeItemComparator);

  // Create Children
  for (const auto& child : node->getChildren()) {
    if (child) {
      _initializeNodesTreeItem(treeItem, child);
    }
  }
}

TreeItem SceneGraphComponent::_createTreeItem(const char* label)
{
  TreeItem treeItem;
  sprintf(treeItem.label, "%s", label);

  return treeItem;
}

TreeItem
SceneGraphComponent::_createMaterialTreeItem(const MaterialPtr& material)
{
  TreeItem treeItem;
  std::string label;
  size_t key = 0;

  if (material) {
    IMaterialTreeItemComponentProps props;
    props.material     = material;
    label              = props.material->name;
    key                = props.material->uniqueId;
    treeItem.component = std::make_shared<MaterialTreeItemComponent>(props);
  }

  sprintf(treeItem.label, "%s", label.c_str());
  sprintf(treeItem.key, "%ld", key);

  return treeItem;
}

TreeItem SceneGraphComponent::_createNodeTreeItem(const NodePtr& node)
{
  TreeItem treeItem;
  std::string label;
  size_t key = 0;

  if (node) {
    if (String::contains(node->getClassName(), "Bone")) {
    }
    else if (String::contains(node->getClassName(), "Camera")) {
      ICameraTreeItemComponentProps props;
      props.camera       = std::static_pointer_cast<Camera>(node);
      label              = props.camera->name;
      key                = props.camera->uniqueId;
      treeItem.component = std::make_shared<CameraTreeItemComponent>(props);
    }
    else if (String::contains(node->getClassName(), "Light")) {
      ILightTreeItemComponentProps props;
      props.light        = std::static_pointer_cast<Light>(node);
      label              = props.light->name;
      key                = props.light->uniqueId;
      treeItem.component = std::make_shared<LightTreeItemComponent>(props);
    }
    else if (String::contains(node->getClassName(), "Mesh")) {
      auto mesh = std::static_pointer_cast<Mesh>(node);
      if (mesh->getTotalVertices() > 0) {
        IMeshTreeItemComponentProps props;
        props.mesh         = std::static_pointer_cast<Mesh>(node);
        label              = props.mesh->name;
        key                = props.mesh->uniqueId;
        treeItem.component = std::make_shared<MeshTreeItemComponent>(props);
      }
    }
  }

  sprintf(treeItem.label, "%s", label.c_str());
  sprintf(treeItem.key, "%ld", key);

  return treeItem;
}

TreeItem SceneGraphComponent::_createSceneTreeItem(Scene* scene)
{
  TreeItem treeItem;

  if (scene) {
    ISceneTreeItemComponentProps props;
    props.scene = scene;
    sprintf(treeItem.key, "%s", "Scene");
    treeItem.component = std::make_shared<SceneTreeItemComponent>(props);
  }

  return treeItem;
}

float SceneGraphComponent::_calculateOffset(unsigned int nodeLevel)
{
  if (nodeLevel < 2) {
    return ImGui::IconSize;
  }

  return (ImGui::IconSizeDouble * nodeLevel);
}

void SceneGraphComponent::notifySelectionChange(EntityInfo& entityInfo)
{
  if (props.globalState->onSelectionChangedObservable.hasObservers()) {
    props.globalState->onSelectionChangedObservable.notifyObservers(
      &entityInfo);
  }
}

void SceneGraphComponent::_renderSelectableTreeItem(TreeNode<TreeItem>& node)
{
  auto& nodeData = node.data();

  if (nodeData.component) {
    if (node.isRoot()) {
      ImGui::Unindent(nodeData.offset);
    }
    else {
      ImGui::Unindent(ImGui::GetStyle().ItemSpacing.x);
    }
    // Make tree item selectable
    ImGui::PushID(nodeData.key);
    if (ImGui::Selectable("", nodeData.isSelected)) {
      DeSelectAll(_sceneGraph.root());
      nodeData.isSelected = !nodeData.isSelected;
      if (nodeData.isSelected) {
        notifySelectionChange(nodeData.component->entityInfo);
      }
    }
    ImGui::PopID();
    // Expandable children
    ImGui::SetItemAllowOverlap();
    if (!node.isRoot() && node.arity() > 0) {
      ImGui::SameLine(nodeData.offset - ImGui::IconSizeDouble);
      ImGui::PushID("TreeItemChildren");
      ImGui::TextWrapped("%s", nodeData.isExpanded ? faMinus : faPlus);
      ImGui::PopID();
      if (ImGui::IsItemHovered()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
      }
      if (ImGui::IsItemClicked(0)) {
        // Switch expanded state
        nodeData.isExpanded = !nodeData.isExpanded;
      }
    }
    ImGui::SameLine(nodeData.offset);
    // Render specialized tree item
    ImGui::SetItemAllowOverlap();
    nodeData.component->render();
    if (node.isRoot()) {
      ImGui::Indent(nodeData.offset);
    }
    else {
      ImGui::Indent(ImGui::GetStyle().ItemSpacing.x);
    }
  }
}

void SceneGraphComponent::_renderChildren(TreeNode<TreeItem>& node)
{
  // Render the specialized tree item selectable component
  _renderSelectableTreeItem(node);

  auto& nodeData = node.data();
  if (node.arity() == 0 || !nodeData.isExpanded) {
    return;
  }

  for (const auto& child : node.children()) {
    child->data().mustExpand = nodeData.mustExpand;
    _renderChildren(*child);
  }
}

void SceneGraphComponent::_renderTree(TreeNode<TreeItem>& node)
{
  auto& nodeData = node.data();

  if (node.depth() == 1) {
    ImGui::Indent(nodeData.offset);
    if (node.isLeaf()) { // Render group containers
      TreeItemRootHeaderComponent::render(nodeData.label);
    }
    else if (!nodeData.isExpanded) { // Render expandable header
      TreeItemExpandableHeaderComponent::render(
        nodeData.isExpanded, nodeData.label, nodeData.mustExpand);
    }
    else {
      // Render expandable header
      TreeItemExpandableHeaderComponent::render(
        nodeData.isExpanded, nodeData.label, nodeData.mustExpand);
      for (const auto& child : node.children()) {
        child->data().mustExpand = nodeData.mustExpand;
      }
      // Render tree item specialized components
      _renderChildren(node);
    }
    ImGui::Unindent(nodeData.offset);
  }
  else {
    // Render the scene tree item selectable component
    _renderSelectableTreeItem(node);

    // Render the scene graph
    for (const auto& child : node.children()) {
      child->data().mustExpand = nodeData.mustExpand;
      _renderTree(*child);
    }
  }
}

void SceneGraphComponent::render()
{
  if (!_initialized) {
    reinitialize();
  }

  _renderTree(_sceneGraph.root());
}

} // namespace BABYLON
