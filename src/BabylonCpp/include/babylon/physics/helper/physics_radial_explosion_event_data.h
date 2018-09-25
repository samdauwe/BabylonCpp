#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_RADIAL_EXPLOSION_EVENT_DATA_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_RADIAL_EXPLOSION_EVENT_DATA_H

#include <babylon/babylon_api.h>
#include <babylon/culling/ray.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

struct BABYLON_SHARED_EXPORT PhysicsRadialExplosionEventData {
  MeshPtr sphere;
  std::vector<Ray> rays;
}; // end of struct PhysicsRadialExplosionEventData

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_HELPER_PHYSICS_RADIAL_EXPLOSION_EVENT_DATA_H
