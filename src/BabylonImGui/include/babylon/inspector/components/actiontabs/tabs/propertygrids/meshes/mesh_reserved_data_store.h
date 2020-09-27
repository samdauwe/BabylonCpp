#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_MESH_RESERVED_DATA_STORE_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_MESH_RESERVED_DATA_STORE_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class AbstractMesh;
class LinesMesh;
class Material;
using AbstractMeshPtr = std::shared_ptr<AbstractMesh>;
using LinesMeshPtr    = std::shared_ptr<LinesMesh>;
using MaterialPtr     = std::shared_ptr<Material>;

struct BABYLON_SHARED_EXPORT MeshReservedDataStore {
  bool hidden                  = false;
  bool displayNormals          = false;
  bool renderGridEnabled       = false;
  bool renderWireframeOver     = false;
  bool renderNormalVectors     = false;
  bool normalMaterialHidden    = true;
  bool isInspectorGrid         = false;
  bool isVertexColorMaterial   = false;
  bool displayVertexColors     = false;
  AbstractMeshPtr gridMesh     = nullptr;
  LinesMeshPtr normalLines     = nullptr;
  MaterialPtr originalMaterial = nullptr;
}; // end of struct MeshReservedDataStore

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_MESH_RESERVED_DATA_STORE_H
