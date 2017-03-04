#ifndef OIMO_CONSTRAINT_JOINT_JOINT_CONFIG_H
#define OIMO_CONSTRAINT_JOINT_JOINT_CONFIG_H

#include <oimo/math/vec3.h>

namespace OIMO {

class RigidBody;

/**
 * @brief A joint configuration holds all configuration data for constructing a
 * joint. Joint configurations can be reused safely.
 */
struct JointConfig {
  // Scale
  float scale    = 1.f;
  float invScale = 1.f;
  // The first rigid body of the joint.
  RigidBody* body1 = nullptr;
  // The second rigid body of the joint.
  RigidBody* body2 = nullptr;
  // The anchor point on the first rigid body in local coordinate system.
  Vec3 localAnchorPoint1;
  //  The anchor point on the second rigid body in local coordinate system.
  Vec3 localAnchorPoint2;
  // The axis in the first body's coordinate system.
  // his property is available in some joints.
  Vec3 localAxis1;
  // The axis in the second body's coordinate system.
  // This property is available in some joints.
  Vec3 localAxis2;
  //  Whether allow collision between connected rigid bodies or not.
  bool allowCollision = false;
}; // end of struct JointConfig

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_JOINT_JOINT_CONFIG_H
