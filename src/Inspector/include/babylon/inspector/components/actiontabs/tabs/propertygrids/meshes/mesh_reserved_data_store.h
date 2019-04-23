#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_MESH_RESERVED_DATA_STORE_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_MESH_RESERVED_DATA_STORE_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class LinesMesh;
class Material;
using LinesMeshPtr = std::shared_ptr<LinesMesh>;
using MaterialPtr  = std::shared_ptr<Material>;

struct BABYLON_SHARED_EXPORT MeshReservedDataStore {
  bool displayNormals          = false;
  bool renderNormalVectors     = false;
  bool renderWireframeOver     = false;
  bool normalMaterialHidden    = true;
  LinesMeshPtr normalLines     = nullptr;
  MaterialPtr originalMaterial = nullptr;
}; // end of struct MeshReservedDataStore

} // end of namespace BABYLON

#endif // end of
       // BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_MESHES_MESH_RESERVED_DATA_STORE_H
