#ifndef BABYLON_CULLING_ICULLABLE_H
#define BABYLON_CULLING_ICULLABLE_H

#include <array>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Plane;

/**
 * @brief Interface for cullable objects.
 * @see https://doc.babylonjs.com/babylon101/materials#back-face-culling
 */
struct ICullable {
  /**
   * @brief Checks if the object or part of the object is in the frustum.
   * @param frustumPlanes Camera near/planes
   * @returns true if the object is in frustum otherwise false
   */
  virtual bool isInFrustum(const std::array<Plane, 6>& frustumPlanes,
                           unsigned int strategy = 0)
    = 0;

  /**
   * @brief Checks if a cullable object (mesh...) is in the camera frustum.
   * Unlike isInFrustum this cheks the full bounding box
   * @param frustumPlanes Camera near/planes
   * @returns true if the object is in frustum otherwise false
   */
  virtual bool
  isCompletelyInFrustum(const std::array<Plane, 6>& frustumPlanes) const = 0;
}; // end of struct ICullable

} // end of namespace BABYLON

#endif // end of BABYLON_CULLING_ICULLABLE_H
