#include <babylon/inspector/components/actiontabs/tabs/property_grid_tab_component.h>

#include <babylon/inspector/components/actiontabs/lines/text_line_component.h>
#include <babylon/inspector/components/actiontabs/tabs/propertygrids/scene_property_grid_component.h>

#include <babylon/core/tree.h>

namespace BABYLON {

struct NodeData {
  NodeData(const char* iName) : name{iName}
  {
  }

  const char* name = nullptr;
  char icon        = '?';
  bool isActive    = false;
};

static Tree<NodeData> tree(NodeData("Scene"));

PropertyGridTabComponent::PropertyGridTabComponent(
  const IPaneComponentProps& iProps)
    : PaneComponent{iProps}, _scenePropertyGridComponent{nullptr}
{
  componentWillMount();
}

PropertyGridTabComponent::~PropertyGridTabComponent()
{
  componentWillUnmount();
}

void PropertyGridTabComponent::componentWillMount()
{
  IScenePropertyGridComponentProps scenePropertyGridComponentProps;
  scenePropertyGridComponentProps.scene = props.scene;
  _scenePropertyGridComponent = std::make_unique<ScenePropertyGridComponent>(
    scenePropertyGridComponentProps);
  {
    tree.root().addChild(NodeData("Nodes")).addChild(NodeData("A node"));
    tree.root()
      .addChild(NodeData("Materials"))
      .addChild(NodeData("A material"));
    tree.root().addChild(NodeData("Textures")).addChild(NodeData("A texture"));
    tree.root().addChild(NodeData("Render pipelines"));
  }
}

void PropertyGridTabComponent::componentWillUnmount()
{
}

static void deSelectAll()
{
  Tree<NodeData>::recursive_visit(
    tree.root(),
    [](TreeNode<NodeData>& nodeData) { nodeData.data().isActive = false; });
}

/**
 * @see https://github.com/ocornut/imgui/issues/1082
 */
static void DisplayTree(TreeNode<NodeData>& node)
{

  ImGuiTreeNodeFlags node_flags
    = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
  if (node.isLeaf()) {
    if (node.depth() == 1) {
      node_flags |= ImGuiTreeNodeFlags_Leaf;
      if (ImGui::TreeNodeEx(node.data().name, node_flags)) {
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
      if (ImGui::Selectable(nodeData.name, nodeData.isActive)) {
        if (!nodeData.isActive) {
          deSelectAll();
          nodeData.isActive = !nodeData.isActive;
          std::cout << nodeData.name << std::endl;
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
    if (ImGui::TreeNodeEx(node.data().name, node_flags)) {
      if (ImGui::IsItemClicked() && node.isRoot() && !node.data().isActive) {
        deSelectAll();
        auto& nodeData    = node.data();
        nodeData.isActive = !nodeData.isActive;
      }
      // ADDED THESE TWO LINES
      ImGui::NextColumn();
      ImGui::NextColumn();

      for (const auto& child : node.children()) {
        DisplayTree(*child);
      }
      ImGui::TreePop();
    }
  }
}

void PropertyGridTabComponent::render()
{
#if 0
  if (_scenePropertyGridComponent) {
    _scenePropertyGridComponent->render();
  }
#else
  {
    ImGui::Separator();
    ImGui::Columns(2);
    ImGui::SetNextTreeNodeOpen(true);
    ImGui::Unindent();
    DisplayTree(tree.root());
    ImGui::Indent();
    ImGui::Columns(1);
  }
#endif
}

} // end of namespace BABYLON
