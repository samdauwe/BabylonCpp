#ifndef BABYLON_PHYSICS_HELPER_PHYSICS_HELPER_ENUMS_H
#define BABYLON_PHYSICS_HELPER_PHYSICS_HELPER_ENUMS_H

namespace BABYLON {

/**
 * @brief The strenght of the force in correspondence to the distance of the
 * affected object
 */
enum class PhysicsRadialImpulseFalloff {
  // impulse is constant in strength across it's whole radius
  Constant = 0,
  // impulse gets weaker if it's further from the origin
  Linear = 1
}; // end of enum class PhysicsRadialImpulseFalloff

/**
 * @brief The strenght of the force in correspondence to the distance of the
 * affected object
 */
enum class PhysicsUpdraftMode {
  // the upstream forces will pull towards the top center of the cylinder
  Center = 0,
  // once a impostor is inside the cylinder, it will shoot out perpendicular
  // from the ground of the cylinder
  Perpendicular = 1
}; // end of enum class PhysicsUpdraftMode

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_HELPER_PHYSICS_HELPER_ENUMS_H