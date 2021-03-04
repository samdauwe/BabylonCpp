#ifndef BABYLON_MESHES_INTERNAL_MESH_DATA_INFO_H
#define BABYLON_MESHES_INTERNAL_MESH_DATA_INFO_H

#include <memory>
#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>
#include <babylon/misc/observable.h>

namespace BABYLON {

class Mesh;
class SubMesh;
FWD_CLASS_SPTR(MeshLODLevel)
FWD_CLASS_SPTR(MorphTargetManager)

/**
 * @brief Hidden
 */
struct BABYLON_SHARED_EXPORT _InternalMeshDataInfo {
  // Events
  Observable<Mesh> _onBeforeRenderObservable;
  Observable<Mesh> _onBeforeBindObservable;
  Observable<Mesh> _onAfterRenderObservable;
  Observable<Mesh> _onBeforeDrawObservable;
  Observable<SubMesh> _onBetweenPassObservable;

  // Will be used by ribbons mainly
  bool _areNormalsFrozen = false;
  // Will be used to save original positions when using software skinning
  Float32Array _sourcePositions;
  // Will be used to save original normals when using software skinning
  Float32Array _sourceNormals;

  // Will be used to save a source mesh reference, If any
  Mesh* _source = nullptr;
  // Will be used to for fast cloned mesh lookup
  std::unordered_map<std::string, Mesh*> meshMap;

  int _preActivateId = -1;
  std::vector<MeshLODLevelPtr> _LODLevels;

  // Morph
  MorphTargetManagerPtr _morphTargetManager = nullptr;
}; // end of struct _InternalMeshDataInfo

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_INTERNAL_MESH_DATA_INFO_H
