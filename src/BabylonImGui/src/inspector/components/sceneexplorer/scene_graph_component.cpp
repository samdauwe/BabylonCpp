#include <babylon/inspector/components/sceneexplorer/scene_graph_component.h>

#include <imgui.h>

#include <babylon/bones/skeleton.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/string.h>
#include <babylon/engines/node.h>
#include <babylon/engines/scene.h>
#include <babylon/lights/light.h>
#include <babylon/materials/material.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/meshes/mesh.h>

#include <imgui_utils/imgui_utils.h>

#include <babylon/inspector/components/global_state.h>
#include <babylon/inspector/components/sceneexplorer/entities/camera_tree_item_component.h>
#include <babylon/inspector/components/sceneexplorer/entities/light_tree_item_component.h>
#include <babylon/inspector/components/sceneexplorer/entities/material_tree_item_component.h>
#include <babylon/inspector/components/sceneexplorer/entities/mesh_tree_item_component.h>
#include <babylon/inspector/components/sceneexplorer/entities/scene_tree_item_component.h>
#include <babylon/inspector/components/sceneexplorer/entities/texture_tree_item_component.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_expandable_header_component.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_root_header_component.h>

namespace BABYLON {

SceneGraphComponent::SceneGraphComponent(const SceneGraphComponentProps& iProps)
    : props{iProps}
    , _sceneGraph{_createSceneTreeItem(iProps.scene)}
    , _initialized{false}
{
  _treeItemComparator = [](const TreeItem& a, const TreeItem& b) -> bool {
    const auto labelA = String::toLowerCase(a.label);
    const auto labelB = String::toLowerCase(b.label);
    return labelA < labelB;
  };
}

SceneGraphComponent::SceneGraphComponent::~SceneGraphComponent()
{
}

void SceneGraphComponent::reinitialize()
{
  const auto& scene = props.scene;

  std::vector<BaseTexturePtr> textures;
  for (auto& texture : scene->textures) {
    if (texture->getClassName() != "AdvancedDynamicTexture") {
      textures.emplace_back(texture);
    }
  }

  // Nodes
  /*const auto& rootNodes = scene->rootNodes;
  auto& nodesTreeItem   = _sceneGraph.root().addChild(_createTreeItem("Nodes"));
  for (const auto& rootNode : rootNodes) {
    _initializeNodesTreeItem(nodesTreeItem, rootNode);
  }*/
  // Materials
  const auto& materials = scene->materials;
  auto& materialsTreeItem
    = _sceneGraph.root().addChild(_createTreeItem("Materials"));
  for (const auto& material : materials) {
    materialsTreeItem.addChildSorted(
      _createMaterialTreeItem(std::static_pointer_cast<Material>(material)),
      _treeItemComparator);
  }
  // Textures
  auto& texturesTreeItem
    = _sceneGraph.root().addChild(_createTreeItem("Textures"));
  for (const auto& texture : textures) {
    texturesTreeItem.addChildSorted(
      _createTextureTreeItem(std::static_pointer_cast<BaseTexture>(texture)),
      _treeItemComparator);
  }
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
    IMaterialTreeItemComponentProps compProps;
    compProps.material = material;
    label              = compProps.material->name;
    key                = compProps.material->uniqueId;
    treeItem.component = std::make_shared<MaterialTreeItemComponent>(compProps);
  }

  sprintf(treeItem.label, "%s", label.c_str());
  sprintf(treeItem.key, "%zd", key);

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
      ICameraTreeItemComponentProps compProps;
      compProps.camera   = std::static_pointer_cast<Camera>(node);
      label              = compProps.camera->name;
      key                = compProps.camera->uniqueId;
      treeItem.component = std::make_shared<CameraTreeItemComponent>(compProps);
    }
    else if (String::contains(node->getClassName(), "Light")) {
      ILightTreeItemComponentProps compProps;
      compProps.light    = std::static_pointer_cast<Light>(node);
      label              = compProps.light->name;
      key                = compProps.light->uniqueId;
      treeItem.component = std::make_shared<LightTreeItemComponent>(compProps);
    }
    else if (String::contains(node->getClassName(), "Mesh")) {
      auto mesh = std::static_pointer_cast<Mesh>(node);
      if (mesh->getTotalVertices() > 0) {
        IMeshTreeItemComponentProps compProps;
        compProps.mesh     = std::static_pointer_cast<Mesh>(node);
        label              = compProps.mesh->name;
        key                = compProps.mesh->uniqueId;
        treeItem.component = std::make_shared<MeshTreeItemComponent>(compProps);
      }
    }
  }

  sprintf(treeItem.label, "%s", label.c_str());
  sprintf(treeItem.key, "%zd", key);

  return treeItem;
}

TreeItem SceneGraphComponent::_createSceneTreeItem(Scene* scene)
{
  TreeItem treeItem;

  if (scene) {
    ISceneTreeItemComponentProps compProps;
    compProps.scene = scene;
    sprintf(treeItem.key, "%s", "Scene");
    treeItem.component = std::make_shared<SceneTreeItemComponent>(compProps);
  }

  return treeItem;
}

TreeItem
SceneGraphComponent::_createTextureTreeItem(const BaseTexturePtr& texture)
{
  TreeItem treeItem;

  if (texture) {
    ITextureTreeItemComponentProps compProps;
    compProps.texture = texture;
    sprintf(treeItem.label, "%s", texture->name.c_str());
    sprintf(treeItem.key, "%s", texture->name.c_str());
    treeItem.component = std::make_shared<TextureTreeItemComponent>(compProps);
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
