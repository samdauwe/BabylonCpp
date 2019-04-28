#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_TRANSFORM_NODE_TREE_ITEM_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_TRANSFORM_NODE_TREE_ITEM_COMPONENT_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/imgui/icons_font_awesome_5.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_specialized_component.h>

namespace BABYLON {

class TransformNode;
using TransformNodePtr = std::shared_ptr<TransformNode>;

struct ITransformNodeTreeItemComponentProps {
  TransformNodePtr transformNode = nullptr;
  std::function<void()> onClick  = nullptr;
}; // end of struct ITransformNodeTreeItemComponentProps

class BABYLON_SHARED_EXPORT TransformNodeTreeItemComponent
    : public TreeItemSpecializedComponent {

public:
  /** Font Awesome Icon Unicodes **/
  static constexpr const char* faCodeBranch = ICON_FA_CODE_BRANCH;

public:
  TransformNodeTreeItemComponent(
    const ITransformNodeTreeItemComponentProps& props);
  virtual ~TransformNodeTreeItemComponent();

  void render() override;

public:
  ITransformNodeTreeItemComponentProps props;

}; // end of class TransformNodeTreeItemComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_TRANSFORM_NODE_TREE_ITEM_COMPONENT_H
