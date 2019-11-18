#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_TEXTURE_TREE_ITEM_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_TEXTURE_TREE_ITEM_COMPONENT_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/inspector/components/sceneexplorer/tree_item_specialized_component.h>
#include <imgui_utils/icons_font_awesome_5.h>

namespace BABYLON {

class BaseTexture;
using BaseTexturePtr = std::shared_ptr<BaseTexture>;

struct ITextureTreeItemComponentProps {
  BaseTexturePtr texture        = nullptr;
  std::function<void()> onClick = nullptr;
}; // end of struct ITextureTreeItemComponentProps

class BABYLON_SHARED_EXPORT TextureTreeItemComponent : public TreeItemSpecializedComponent {

public:
  /** Font Awesome Icon Unicodes **/
  static constexpr const char* faImage = ICON_FA_IMAGE;

public:
  TextureTreeItemComponent(const ITextureTreeItemComponentProps& props);
  ~TextureTreeItemComponent() override; // = default

  void render() override;

public:
  ITextureTreeItemComponentProps props;

}; // end of class TextureTreeItemComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_TEXTURE_TREE_ITEM_COMPONENT_H
