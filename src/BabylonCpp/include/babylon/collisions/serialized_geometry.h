#ifndef BABYLON_COLLISIONS_SERIALIZED_GEOMETRY_H
#define BABYLON_COLLISIONS_SERIALIZED_GEOMETRY_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT SerializedGeometry {
  std::string id;
  Float32Array positions;
  Uint32Array indices;
  Float32Array normals;
  std::vector<Vector3> positionsArray;
}; // end of struct SerializedGeometry

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_SERIALIZED_GEOMETRY_H
