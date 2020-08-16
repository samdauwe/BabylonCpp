#ifndef BABYLON_MISC_INTERFACES_ICLIP_PLANES_HOLDER_H
#define BABYLON_MISC_INTERFACES_ICLIP_PLANES_HOLDER_H

#include <optional>

#include <babylon/maths/plane.h>

namespace BABYLON {

class Plane;

/**
 * @brief Interface used to define entities containing multiple clip planes.
 */
struct IClipPlanesHolder {
  /**
   * Gets or sets the active clipplane 1
   */
  std::optional<Plane> clipPlane = std::nullopt;

  /**
   * Gets or sets the active clipplane 2
   */
  std::optional<Plane> clipPlane2 = std::nullopt;

  /**
   * Gets or sets the active clipplane 3
   */
  std::optional<Plane> clipPlane3 = std::nullopt;

  /**
   * Gets or sets the active clipplane 4
   */
  std::optional<Plane> clipPlane4 = std::nullopt;

  /**
   * Gets or sets the active clipplane 5
   */
  std::optional<Plane> clipPlane5 = std::nullopt;

  /**
   * Gets or sets the active clipplane 6
   */
  std::optional<Plane> clipPlane6 = std::nullopt;
}; // end of struct IClipPlanesHolder

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_INTERFACES_ICLIP_PLANES_HOLDER_H
