#ifndef BABYLON_PHYSICS_JOINT_PHYSICS_JOINT_H
#define BABYLON_PHYSICS_JOINT_PHYSICS_JOINT_H

#include <babylon/babylon_global.h>
#include <babylon/physics/joint/physics_joint_data.h>

namespace BABYLON {

/**
 * @brief This is a holder class for the physics joint created by the physics
 * plugin. It holds a set of functions to control the underlying joint.
 */
class BABYLON_SHARED_EXPORT PhysicsJoint {

public:
  // Joint Types
  static constexpr unsigned int DistanceJoint      = 0;
  static constexpr unsigned int HingeJoint         = 1;
  static constexpr unsigned int BallAndSocketJoint = 2;
  static constexpr unsigned int WheelJoint         = 3;
  static constexpr unsigned int SliderJoint        = 4;
  // OIMO
  static constexpr unsigned int PrismaticJoint = 5;
  // ENERGY FTW! (compare with this -
  // http://ode-wiki.org/wiki/index.php?title=Manual:_Joint_Types_and_Functions)
  static constexpr unsigned int UniversalJoint = 6;
  static constexpr unsigned int Hinge2Joint    = PhysicsJoint::WheelJoint;
  // Cannon
  // Similar to a Ball-Joint. Different in params
  static constexpr unsigned int PointToPointJoint = 8;
  // Cannon only at the moment
  static constexpr unsigned int SpringJoint = 9;
  static constexpr unsigned int LockJoint   = 10;

public:
  PhysicsJoint(unsigned int jointType, const PhysicsJointData& jointData);
  ~PhysicsJoint();

  PhysicsJoint* physicsJoint();
  void setPhysicsJoint(PhysicsJoint* newJoint);
  void setPhysicsPlugin(IPhysicsEnginePlugin* physicsPlugin);

  /**
   * Execute a function that is physics-plugin specific.
   * @param {Function} func the function that will be executed.
   *                        It accepts two parameters: the physics world and the
   * physics joint.
   */
  void executeNativeFunction(
    const std::function<void(IWorld* world, PhysicsJoint* physicsJoint)>& func);

public:
  unsigned int _jointType;

protected:
  IPhysicsEnginePlugin* _physicsPlugin;

private:
  PhysicsJoint* _physicsJoint;
  PhysicsJointData _jointData;

}; // end of class PhysicsJoint

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_JOINT_PHYSICS_JOINT_H
