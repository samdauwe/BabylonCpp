#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_MATERIAL_TREE_ITEM_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_MATERIAL_TREE_ITEM_COMPONENT_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/imgui/icons_font_awesome_5.h>

namespace BABYLON {

class Material;
class Scene;
using MaterialPtr = std::shared_ptr<Material>;

struct IMaterialTreeItemComponentProps {
  MaterialPtr material          = nullptr;
  std::function<void()> onClick = nullptr;
}; // end of struct IMaterialTreeItemComponentProps

class BABYLON_SHARED_EXPORT MaterialTreeItemComponent {

public:
  /** Font Awesome Icon Unicodes **/
  static constexpr const char* faBrush = ICON_FA_BRUSH;

public:
  MaterialTreeItemComponent(const IMaterialTreeItemComponentProps& props);
  ~MaterialTreeItemComponent();

  void render();

public:
  IMaterialTreeItemComponentProps props;

}; // end of class LightTreeItemComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_MATERIAL_TREE_ITEM_COMPONENT_H
