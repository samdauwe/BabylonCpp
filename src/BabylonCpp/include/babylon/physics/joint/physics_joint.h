#ifndef BABYLON_PHYSICS_JOINT_PHYSICS_JOINT_H
#define BABYLON_PHYSICS_JOINT_PHYSICS_JOINT_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/physics/joint/physics_joint_data.h>

namespace BABYLON {

struct IPhysicsEnginePlugin;
class Mesh;

/**
 * @brief This is a holder class for the physics joint created by the physics
 * plugin It holds a set of functions to control the underlying joint.
 * @see https://doc.babylonjs.com/how_to/using_the_physics_engine
 */
class BABYLON_SHARED_EXPORT PhysicsJoint {

public:
  // Joint Types
  /**
   * Distance-Joint type
   */
  static constexpr unsigned int DistanceJoint = 0;
  /**
   * Hinge-Joint type
   */
  static constexpr unsigned int HingeJoint = 1;
  /**
   * Ball-and-Socket joint type
   */
  static constexpr unsigned int BallAndSocketJoint = 2;
  /**
   * Wheel-Joint type
   */
  static constexpr unsigned int WheelJoint = 3;
  /**
   * Slider-Joint type
   */
  static constexpr unsigned int SliderJoint = 4;
  // OIMO
  /**
   * Prismatic-Joint type
   */
  static constexpr unsigned int PrismaticJoint = 5;
  //
  /**
   * Universal-Joint type
   * ENERGY FTW! (compare with this
   * @see
   * http://ode-wiki.org/wiki/index.php?title=Manual:_Joint_Types_and_Functions)
   */
  static constexpr unsigned int UniversalJoint = 6;
  /**
   * Hinge-Joint 2 type
   */
  static constexpr unsigned int Hinge2Joint = PhysicsJoint::WheelJoint;
  // Cannon
  /**
   * Point to Point Joint type.  Similar to a Ball-Joint.  Different in
   * parameters
   */
  static constexpr unsigned int PointToPointJoint = 8;
  // Cannon only at the moment
  /**
   * Spring-Joint type
   */
  static constexpr unsigned int SpringJoint = 9;
  /**
   * Lock-Joint type
   */
  static constexpr unsigned int LockJoint = 10;

public:
  /**
   * @brief Initializes the physics joint.
   * @param type The type of the physics joint
   * @param jointData The data for the physics joint
   */
  PhysicsJoint(unsigned int jointType, const PhysicsJointData& jointData);
  ~PhysicsJoint();

  /**
   * Execute a function that is physics-plugin specific.
   * @param {Function} func the function that will be executed.
   *                        It accepts two parameters: the physics world and the
   * physics joint
   */
  void executeNativeFunction(
    const std::function<void(Mesh* world, PhysicsJoint* physicsJoint)>& func);

protected:
  /**
   * @brief Gets the physics joint.
   */
  PhysicsJoint*& get_physicsJoint();

  /**
   * @brief Sets the physics joint.
   */
  void set_physicsJoint(PhysicsJoint* const& newJoint);

  /**
   * @brief Sets the physics plugin.
   */
  void set_physicsPlugin(IPhysicsEnginePlugin* const& physicsPlugin);

public:
  /**
   * The type of the physics joint
   */
  unsigned int jointType;

  /**
   * The data for the physics joint
   */
  PhysicsJointData jointData;

  /**
   * Gets or sets the physics joint
   */
  Property<PhysicsJoint, PhysicsJoint*> physicsJoint;

  /**
   * Sets the physics plugin
   */
  WriteOnlyProperty<PhysicsJoint, IPhysicsEnginePlugin*> physicsPlugin;

protected:
  IPhysicsEnginePlugin* _physicsPlugin;

private:
  PhysicsJoint* _physicsJoint;

}; // end of class PhysicsJoint

} // end of namespace BABYLON

#endif // end of BABYLON_PHYSICS_JOINT_PHYSICS_JOINT_H
