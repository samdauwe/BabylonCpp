#ifndef BABYLON_COLLISIONS_UPDATE_PAYLOAD_H
#define BABYLON_COLLISIONS_UPDATE_PAYLOAD_H

#include <babylon/babylon_global.h>
#include <babylon/collisions/serialized_geometry.h>
#include <babylon/collisions/serialized_mesh.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT UpdatePayload {
  std::unordered_map<unsigned int, SerializedMesh> updatedMeshes;
  std::unordered_map<string_t, SerializedGeometry> updatedGeometries;
  Uint32Array removedMeshes;
  vector_t<string_t> removedGeometries;
}; // end of struct UpdatePayload

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_UPDATE_PAYLOAD_H
