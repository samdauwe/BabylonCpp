#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_RADIAL_EXPLOSION_EVENT_DATA_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_RADIAL_EXPLOSION_EVENT_DATA_H

#include <babylon/babylon_api.h>
#include <babylon/culling/ray.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

/**
 * @brief Interface for radial explosion event data.
 * @see https://doc.babylonjs.com/how_to/using_the_physics_engine
 */
struct BABYLON_SHARED_EXPORT PhysicsRadialExplosionEventData {
  /**
   * A sphere used for the radial explosion event
   */
  MeshPtr sphere;
  /**
   * An array of rays for the radial explosion event
   */
  std::vector<Ray> rays;
}; // end of struct PhysicsRadialExplosionEventData

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_HELPER_PHYSICS_RADIAL_EXPLOSION_EVENT_DATA_H
