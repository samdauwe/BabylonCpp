#include <babylon/inspector/components/sceneexplorer/entities/post_process_tree_item_component.h>

#include <babylon/core/string.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_label_component.h>
#include <babylon/postprocesses/post_process.h>
#include <imgui_utils/imgui_utils.h>

namespace BABYLON {

PostProcessTreeItemComponent::PostProcessTreeItemComponent(
  const IPostProcessTreeItemComponentProps& iProps)
    : props{iProps}
{
  const auto& postProcess = props.postProcess;

  sprintf(label, "%s", postProcess->name.c_str());

  // Set the entity info
  entityInfo.name = postProcess->name;
  entityInfo.type = EntityType::PostProcess;
}

PostProcessTreeItemComponent::~PostProcessTreeItemComponent()
{
}

void PostProcessTreeItemComponent::render()
{
  // PostProcess tree item label
  TreeItemLabelComponent::render(label, faMagic, ImGui::red);
}

} // end of namespace BABYLON
