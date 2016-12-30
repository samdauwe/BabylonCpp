#ifndef BABYLON_COLLISIONS_SERIALIZED_SUB_MESH_H
#define BABYLON_COLLISIONS_SERIALIZED_SUB_MESH_H

#include <babylon/babylon_global.h>
#include <babylon/math/matrix.h>
#include <babylon/math/plane.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT SerializedSubMesh {
  unsigned int position;
  unsigned int verticesStart;
  size_t verticesCount;
  size_t indexStart;
  size_t indexCount;
  bool hasMaterial;
  Float32Array sphereCenter;
  float sphereRadius;
  Float32Array boxMinimum;
  Float32Array boxMaximum;
  std::vector<Vector3> positionsArray;
  std::vector<Vector3> _lastColliderWorldVertices;
  Matrix _lastColliderTransformMatrix;
  std::vector<Plane> _trianglePlanes;

}; // end of struct SerializedSubMesh

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_SERIALIZED_SUB_MESH_H
