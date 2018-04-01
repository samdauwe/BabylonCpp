#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_RADIAL_EXPLOSION_EVENT_DATA_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_RADIAL_EXPLOSION_EVENT_DATA_H

#include <babylon/babylon_global.h>
#include <babylon/culling/ray.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT PhysicsRadialExplosionEventData {
  Mesh* sphere;
  vector_t<Ray> rays;
}; // end of struct PhysicsRadialExplosionEventData

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_HELPER_PHYSICS_RADIAL_EXPLOSION_EVENT_DATA_H
