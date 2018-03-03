#ifndef BABYLON_COLLISIONS_SERIALIZED_GEOMETRY_H
#define BABYLON_COLLISIONS_SERIALIZED_GEOMETRY_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief Interface describing the value associated with a geometry
 */
struct BABYLON_SHARED_EXPORT SerializedGeometry {
  /**
   * Defines the unique ID of the geometry
   */
  string_t id;
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
  vector_t<Vector3> positionsArray;
}; // end of struct SerializedGeometry

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_SERIALIZED_GEOMETRY_H
