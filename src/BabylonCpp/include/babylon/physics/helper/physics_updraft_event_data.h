#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_UPDRAFT_EVENT_DATA_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_UPDRAFT_EVENT_DATA_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

/**
 * @brief Interface for updraft event data.
 * @see https://doc.babylonjs.com/how_to/using_the_physics_engine
 */
struct BABYLON_SHARED_EXPORT PhysicsUpdraftEventData {
  /**
   * A cylinder used for the updraft event
   */
  MeshPtr cylinder;
}; // end of struct PhysicsUpdraftEventData

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_HELPER_PHYSICS_UPDRAFT_EVENT_DATA_H
