#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_SCENE_GRAPH_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_SCENE_GRAPH_COMPONENT_H

#include <babylon/babylon_api.h>
#include <babylon/core/tree.h>

namespace BABYLON {

struct TreeItem {
  const char* label = nullptr;
  char icon         = '?';
  bool isActive     = false;
}; // end of struct SceneGraphItem

class Node;
class Scene;

class BABYLON_SHARED_EXPORT SceneGraphComponent {

private:
    template <typename T>
    using TreeItemFactoryMethod = std::function<TreeItem(const T& node)>;

public:
  SceneGraphComponent(Scene* scene);
  ~SceneGraphComponent();

  void reinitialize();
  void render();

  /**
   * @brief Deselects all children nodes of the node in the tree.
   */
  static void DeSelectAll(TreeNode<TreeItem>& node);

private:
  /**
   * @brief Initializes the "Nodes" subtree.
   */
  template <typename T>
  void _initializeNodesTreeItem(TreeNode<TreeItem>& parentTreeItem,
                                const T& node)
  {
    // Create TreeItem
    auto& treeItem = parentTreeItem.addChild(TreeItem{node->name.c_str()});

    // Create Children
    for (auto&& child : node->getChildren()) {
      if (child) {
        _initializeTreeItem(treeItem, child.get());
      }
    }
  }

  /**
   * @brief Initializes a subtree.
   */
  template <typename T>
  void _initializeTreeItem(TreeNode<TreeItem>& parentTreeItem, const T& node)
  {
    // Create TreeItem
    parentTreeItem.addChild(TreeItem{node->name.c_str()});
  }

  /**
   * @brief Render the scene graph.
   * @see https://github.com/ocornut/imgui/issues/1082
   */
  void _renderTree(TreeNode<TreeItem>& node);

private:
  Scene* _scene;
  Tree<TreeItem> _sceneGraph;
  bool _initialized;

}; // end of class SceneGraphComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_SCENE_GRAPH_COMPONENT_H
