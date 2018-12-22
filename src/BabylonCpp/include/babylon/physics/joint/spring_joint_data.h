#ifndef BABYLON_PHYSICS_JOINT_SPRING_JOINT_DATA_H
#define BABYLON_PHYSICS_JOINT_SPRING_JOINT_DATA_H

#include <babylon/babylon_api.h>
#include <babylon/physics/joint/physics_joint_data.h>

namespace BABYLON {

/**
 * @brief Joint data from a spring joint.
 * @see https://doc.babylonjs.com/how_to/using_the_physics_engine
 */
struct BABYLON_SHARED_EXPORT SpringJointData : public PhysicsJointData {
  /**
   * Length of the spring
   */
  float length = 0.f;
  /**
   * Stiffness of the spring
   */
  float stiffness = 0.f;
  /**
   * Damping of the spring
   */
  float damping = 0.f;
  /**
   * This callback will be called when applying the force to the impostors.
   */
  std::function<void()> forceApplicationCallback = nullptr;
}; // end of struct DistanceJointData

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_JOINT_SPRING_JOINT_DATA_H
