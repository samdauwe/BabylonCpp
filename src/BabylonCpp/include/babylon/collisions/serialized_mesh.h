#ifndef BABYLON_COLLISIONS_SERIALIZED_MESH_H
#define BABYLON_COLLISIONS_SERIALIZED_MESH_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/collisions/serialized_sub_mesh.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT SerializedMesh {
  std::string id;
  std::string name;
  unsigned int uniqueId;
  std::string geometryId;
  Float32Array sphereCenter;
  float sphereRadius;
  Float32Array boxMinimum;
  Float32Array boxMaximum;
  Float32Array worldMatrixFromCache;
  std::vector<SerializedSubMesh> subMeshes;
  bool checkCollisions;
}; // end of struct SerializedMesh

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_SERIALIZED_MESH_H
