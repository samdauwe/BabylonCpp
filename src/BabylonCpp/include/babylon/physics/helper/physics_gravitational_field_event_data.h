#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_GRAVITATIONAL_FIELD_EVENT_DATA_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_GRAVITATIONAL_FIELD_EVENT_DATA_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

/**
 * @brief Interface for gravitational field event data.
 * @see https://doc.babylonjs.com/how_to/using_the_physics_engine
 */
struct BABYLON_SHARED_EXPORT PhysicsGravitationalFieldEventData {
  /**
   * A sphere mesh used for the gravitational field event
   */
  MeshPtr sphere;
}; // end of struct PhysicsGravitationalFieldEventData

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_HELPER_PHYSICS_GRAVITATIONAL_FIELD_EVENT_DATA_H
