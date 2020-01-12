#include <babylon/inspector/components/sceneexplorer/entities/texture_tree_item_component.h>

#include <babylon/inspector/components/sceneexplorer/tree_item_label_component.h>
#include <babylon/materials/textures/texture.h>
#include <babylon/misc/string_tools.h>
#include <imgui_utils/imgui_utils.h>

namespace BABYLON {

TextureTreeItemComponent::TextureTreeItemComponent(const ITextureTreeItemComponentProps& iProps)
    : props{iProps}
{
  const auto& texture = props.texture;

  sprintf(label, "%s", texture->name.c_str());

  // Set the entity info
  const auto& className = texture->getClassName();
  entityInfo.name       = texture->name;
  if (StringTools::contains(className, "Texture")) {
    entityInfo.type = EntityType::Texture;
  }
}

TextureTreeItemComponent::~TextureTreeItemComponent() = default;

void TextureTreeItemComponent::render()
{
  // Texture tree item label
  TreeItemLabelComponent::render(label, faImage, ImGui::mediumpurple);
}

} // end of namespace BABYLON
