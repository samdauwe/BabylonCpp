#ifndef BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_MESH_TREE_ITEM_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_MESH_TREE_ITEM_COMPONENT_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/imgui/icons_font_awesome.h>

namespace BABYLON {

class AbstractMesh;
class Scene;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;

struct IMeshTreeItemComponentProps {
  AbstractMeshPtr mesh          = nullptr;
  std::function<void()> onClick = nullptr;
}; // end of struct IMeshTreeItemComponentProps

struct MeshTreeItemComponentState {
  bool isVisible      = false;
  bool isGizmoEnabled = false;
}; // end of struct MeshTreeItemComponentState

class BABYLON_SHARED_EXPORT MeshTreeItemComponent {

public:
  /** Font Awesome Icon Unicodes **/
  // fa-cube
  static constexpr const char* faCube = ICON_FA_CUBE;
  // fa-eye
  static constexpr const char* faEye = ICON_FA_EYE;
  // fa-eye-slash
  static constexpr const char* faEyeSlash = ICON_FA_EYE_SLASH;
  // fa-eye-slash
  static constexpr const char* faVectorSquare = ICON_FA_SQUARE_O;

public:
  MeshTreeItemComponent(const IMeshTreeItemComponentProps& props);
  ~MeshTreeItemComponent();

  void showGizmos();
  void switchVisibility();
  void render();

public:
  IMeshTreeItemComponentProps props;
  MeshTreeItemComponentState state;

}; // end of class LightTreeItemComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_SCENE_EXPLORER_ENTITIES_MESH_TREE_ITEM_COMPONENT_H
