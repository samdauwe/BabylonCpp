#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_VORTEX_EVENT_DATA_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_VORTEX_EVENT_DATA_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

struct BABYLON_SHARED_EXPORT PhysicsVortexEventData {
  MeshPtr cylinder;
}; // end of struct PhysicsVortexEventData

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_HELPER_PHYSICS_VORTEX_EVENT_DATA_H
