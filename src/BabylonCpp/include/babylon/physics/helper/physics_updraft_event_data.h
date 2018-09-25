#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_UPDRAFT_EVENT_DATA_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_UPDRAFT_EVENT_DATA_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

struct BABYLON_SHARED_EXPORT PhysicsUpdraftEventData {
  MeshPtr cylinder;
}; // end of struct PhysicsUpdraftEventData

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_HELPER_PHYSICS_UPDRAFT_EVENT_DATA_H
