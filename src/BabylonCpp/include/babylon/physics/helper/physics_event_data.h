#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_EVENT_DATA_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_EVENT_DATA_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Mesh;
using MeshPtr = std::shared_ptr<Mesh>;

/**
 * @brief Interface for radial explosion event data.
 * @see
 * https://doc.babylonjs.com/how_to/using_the_physics_engine#further-functionality-of-the-impostor-class
 */
struct BABYLON_SHARED_EXPORT PhysicsRadialExplosionEventData {
  /**
   * A sphere used for the radial explosion event
   */
  MeshPtr sphere = nullptr;
}; // end of struct PhysicsRadialExplosionEventData

/**
 * @brief Interface for gravitational field event data.
 * @see
 * https://doc.babylonjs.com/how_to/using_the_physics_engine#further-functionality-of-the-impostor-class
 */
struct BABYLON_SHARED_EXPORT PhysicsGravitationalFieldEventData {
  /**
   * A sphere mesh used for the gravitational field event
   */
  MeshPtr sphere = nullptr;
}; // end of struct PhysicsGravitationalFieldEventData

/**
 * @brief Interface for updraft event data.
 * @see
 * https://doc.babylonjs.com/how_to/using_the_physics_engine#further-functionality-of-the-impostor-class
 */
struct BABYLON_SHARED_EXPORT PhysicsUpdraftEventData {
  /**
   * A cylinder used for the updraft event
   */
  MeshPtr cylinder = nullptr;
}; // end of struct PhysicsUpdraftEventData

/**
 * @brief Interface for vortex event data.
 * @see
 * https://doc.babylonjs.com/how_to/using_the_physics_engine#further-functionality-of-the-impostor-class
 */
struct BABYLON_SHARED_EXPORT PhysicsVortexEventData {
  /**
   * A cylinder used for the vortex event
   */
  MeshPtr cylinder = nullptr;
}; // end of struct PhysicsVortexEventData

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_HELPER_PHYSICS_EVENT_DATA_H
