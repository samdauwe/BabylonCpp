#ifndef BABYLON_COLLISIONS_SERIALIZED_GEOMETRY_H
#define BABYLON_COLLISIONS_SERIALIZED_GEOMETRY_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class Vector3;

/**
 * @brief Interface describing the value associated with a geometry
 * Hidden
 */
struct BABYLON_SHARED_EXPORT SerializedGeometry {
  /**
   * Defines the unique ID of the geometry
   */
  std::string id;
  /**
   * Defines the array containing the positions
   */
  Float32Array positions;
  /**
   * Defines the array containing the indices
   */
  Uint32Array indices;
  /**
   * Defines the array containing the normals
   */
  Float32Array normals;
  std::vector<Vector3> positionsArray;
}; // end of struct SerializedGeometry

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_SERIALIZED_GEOMETRY_H
