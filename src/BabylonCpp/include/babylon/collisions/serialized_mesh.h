#ifndef BABYLON_COLLISIONS_SERIALIZED_MESH_H
#define BABYLON_COLLISIONS_SERIALIZED_MESH_H

#include <babylon/babylon_global.h>
#include <babylon/collisions/serialized_sub_mesh.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT SerializedMesh {
  string_t id;
  string_t name;
  unsigned int uniqueId;
  string_t geometryId;
  Float32Array sphereCenter;
  float sphereRadius;
  Float32Array boxMinimum;
  Float32Array boxMaximum;
  Float32Array worldMatrixFromCache;
  vector_t<SerializedSubMesh> subMeshes;
  bool checkCollisions;
}; // end of struct SerializedMesh

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_SERIALIZED_MESH_H
