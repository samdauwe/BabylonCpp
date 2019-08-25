#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_RENDERING_PIPELINE_TREE_ITEM_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_RENDERING_PIPELINE_TREE_ITEM_COMPONENT_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <imgui_utils/icons_font_awesome_5.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_specialized_component.h>

namespace BABYLON {

class PostProcessRenderPipeline;
using PostProcessRenderPipelinePtr = std::shared_ptr<PostProcessRenderPipeline>;

struct IRenderPipelineItemComponentProps {
  PostProcessRenderPipelinePtr renderPipeline = nullptr;
  std::function<void()> onClick               = nullptr;
}; // end of struct IRenderPipelineItemComponentProps

class BABYLON_SHARED_EXPORT RenderingPipelineTreeItemComponent
    : public TreeItemSpecializedComponent {

public:
  /** Font Awesome Icon Unicodes **/
  static constexpr const char* faMagic = ICON_FA_MAGIC;

public:
  RenderingPipelineTreeItemComponent(
    const IRenderPipelineItemComponentProps& props);
  virtual ~RenderingPipelineTreeItemComponent();

  void render() override;

public:
  IRenderPipelineItemComponentProps props;

}; // end of class RenderingPipelineItemComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_RENDERING_PIPELINE_TREE_ITEM_COMPONENT_H
