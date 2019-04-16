#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_SCENE_GRAPH_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_SCENE_GRAPH_COMPONENT_H

#include <babylon/babylon_api.h>
#include <babylon/core/string.h>
#include <babylon/core/tree.h>

namespace BABYLON {

class Material;
class Node;
class Scene;
struct TreeItemSpecializedComponent;
using MaterialPtr = std::shared_ptr<Material>;
using NodePtr     = std::shared_ptr<Node>;
using TreeItemSpecializedComponentPtr
  = std::shared_ptr<TreeItemSpecializedComponent>;
using TreeItem = TreeItemSpecializedComponentPtr;

class BABYLON_SHARED_EXPORT SceneGraphComponent {

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
   * @brief Initializes a subtree.
   */
  template <typename T>
  void _initializeTreeItem(TreeNode<TreeItem>& parentTreeItem, const T& entity)
  {
    if (String::contains(entity->getClassName(), "Material")) {
      parentTreeItem.addChildSorted(
        createMaterialTreeItem(std::static_pointer_cast<Material>(entity)),
        _treeItemComparator);
    }
    else {
      // Create TreeItem
      parentTreeItem.addChild(createGroupContainer(entity->name.c_str()));
    }
  }

  /**
   * @brief Initializes the "Nodes" subtree.
   */
  void _initializeNodesTreeItem(TreeNode<TreeItem>& parentTreeItem,
                                const NodePtr& node);

  TreeItem createGroupContainer(const char* label);
  TreeItem createMaterialTreeItem(const MaterialPtr& material);
  TreeItem createNodeTreeItem(const NodePtr& node);
  TreeItem createSceneTreeItem(Scene* scene);

  /**
   * @brief Render the scene graph.
   * @see https://github.com/ocornut/imgui/issues/1082
   */
  void _renderTree(TreeNode<TreeItem>& node);

private:
  Scene* _scene;
  Tree<TreeItem> _sceneGraph;
  bool _initialized;
  std::function<bool(const TreeItem& a, const TreeItem& b)> _treeItemComparator;

}; // end of class SceneGraphComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_SCENE_GRAPH_COMPONENT_H
