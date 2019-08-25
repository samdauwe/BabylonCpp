#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_POST_PROCESS_TREE_ITEM_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_POST_PROCESS_TREE_ITEM_COMPONENT_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <imgui_utils/icons_font_awesome_5.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_specialized_component.h>

namespace BABYLON {

class PostProcess;
using PostProcessPtr = std::shared_ptr<PostProcess>;

struct IPostProcessTreeItemComponentProps {
  PostProcessPtr postProcess    = nullptr;
  std::function<void()> onClick = nullptr;
}; // end of struct IPostProcessItemComponentProps

class BABYLON_SHARED_EXPORT PostProcessTreeItemComponent
    : public TreeItemSpecializedComponent {

public:
  /** Font Awesome Icon Unicodes **/
  static constexpr const char* faMagic = ICON_FA_MAGIC;

public:
  PostProcessTreeItemComponent(const IPostProcessTreeItemComponentProps& props);
  virtual ~PostProcessTreeItemComponent();

  void render() override;

public:
  IPostProcessTreeItemComponentProps props;

}; // end of class PostProcessTreeItemComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_POST_PROCESS_TREE_ITEM_COMPONENT_H
