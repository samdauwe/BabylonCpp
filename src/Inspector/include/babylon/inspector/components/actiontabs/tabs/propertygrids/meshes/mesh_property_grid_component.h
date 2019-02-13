#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_MESH_PROPERTY_GRID_COMPONENT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_MESH_PROPERTY_GRID_COMPONENT_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

struct IMeshPropertyGridComponentProps {
  MeshPtr mesh;
}; // end of IMeshPropertyGridComponentProps

struct MeshPropertyGridComponentState {
  bool displayNormals      = false;
  bool renderNormalVectors = false;
}; // end of MeshPropertyGridComponentState

class BABYLON_SHARED_EXPORT MeshPropertyGridComponent {

public:
  MeshPropertyGridComponent(const IMeshPropertyGridComponentProps& props);
  ~MeshPropertyGridComponent();

  void renderNormalVectors();
  void displayNormals();
  void onMaterialLink();
  std::string convertPhysicsTypeToString() const;
  void render();

public:
  IMeshPropertyGridComponentProps props;
  MeshPropertyGridComponentState state;

}; // end of class MeshPropertyGridComponent

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_MESH_PROPERTY_GRID_COMPONENT_H
