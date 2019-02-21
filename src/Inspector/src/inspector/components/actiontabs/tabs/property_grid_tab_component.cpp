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
    tree.root()
      .addChild(NodeData("Render pipelines"))
      .addChild(NodeData("A render pipeline"));
  }
}

void PropertyGridTabComponent::componentWillUnmount()
{
}

/**
 * @see https://github.com/ocornut/imgui/issues/1082
 */
static void DisplayTree(TreeNode<NodeData>& node)
{

  if (node.children().size() == 0) {
    // Render tree item
    ImGui::Text(">");
    ImGui::SameLine();
    auto& nodeData = node.data();
    if (ImGui::Selectable(nodeData.name, nodeData.isActive)) {
      if (!nodeData.isActive) {
        Tree<NodeData>::recursive_visit(tree.root(),
                                        [](TreeNode<NodeData>& nodeData) {
                                          nodeData.data().isActive = false;
                                        });
        std::cout << nodeData.name << std::endl;
      }
      nodeData.isActive = !nodeData.isActive;
    }
    ImGui::NextColumn();

    ImGui::Button(nodeData.name);
    ImGui::NextColumn();
  }
  else {
    ImGui::AlignFirstTextHeightToWidgets(); // not needed to solve bug. Helps
                                            // make vertical spacing more
                                            // consistent
    if (ImGui::TreeNode(node.data().name)) {
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
    DisplayTree(tree.root());
    ImGui::Columns(1);
  }
#endif
}

} // end of namespace BABYLON
