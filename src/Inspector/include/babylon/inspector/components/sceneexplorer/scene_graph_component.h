#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_SCENE_GRAPH_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_SCENE_GRAPH_COMPONENT_H

#include <babylon/babylon_api.h>
#include <babylon/core/string.h>
#include <babylon/core/tree.h>
#include <babylon/imgui/icons_font_awesome_5.h>

#include <babylon/inspector/components/sceneexplorer/tree_item_component.h>

namespace BABYLON {

class Material;
class Node;
class Scene;
struct TreeItemComponent;
using MaterialPtr = std::shared_ptr<Material>;
using NodePtr     = std::shared_ptr<Node>;
using TreeItem    = TreeItemComponent;

class BABYLON_SHARED_EXPORT SceneGraphComponent {

public:
  /** Font Awesome Icon Unicodes **/
  static constexpr const char* faMinus = ICON_FA_MINUS;
  static constexpr const char* faPlus  = ICON_FA_PLUS;

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
        _createMaterialTreeItem(std::static_pointer_cast<Material>(entity)),
        _treeItemComparator);
    }
  }

  /**
   * @brief Initializes the "Nodes" subtree.
   */
  void _initializeNodesTreeItem(TreeNode<TreeItem>& parentTreeItem,
                                const NodePtr& node);

  TreeItem _createTreeItem(const char* label);
  TreeItem _createMaterialTreeItem(const MaterialPtr& material);
  TreeItem _createNodeTreeItem(const NodePtr& node);
  TreeItem _createSceneTreeItem(Scene* scene);

  float _calculateOffset(unsigned int nodeLevel);

  /**
   * @brief Render a selectable tree item.
   * @param node
   */
  void _renderSelectableTreeItem(TreeNode<TreeItem>& node);

  /**
   * @brief Renders the selectable children
   */
  void _renderChildren(TreeNode<TreeItem>& node);

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
