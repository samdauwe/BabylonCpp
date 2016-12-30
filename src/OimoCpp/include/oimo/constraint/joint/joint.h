#ifndef OIMO_CONSTRAINT_JOINT_JOINT_JOINT_H
#define OIMO_CONSTRAINT_JOINT_JOINT_JOINT_H

#include <string>
#include <utility>

#include <oimo/constraint/constraint.h>
#include <oimo/math/mat44.h>
#include <oimo/math/vec3.h>

namespace OIMO {

struct JointConfig;
struct JointLink;
class RigidBody;

/**
 * @brief Joints are used to constrain the motion between two rigid bodies.
 */
class Joint : public Constraint {

public:
  // Joint type
  enum class Type : unsigned int {
    JOINT_NULL            = 0,
    JOINT_DISTANCE        = 1,
    JOINT_BALL_AND_SOCKET = 2,
    JOINT_HINGE           = 3,
    JOINT_WHEEL           = 4,
    JOINT_SLIDER          = 5,
    JOINT_PRISMATIC       = 6
  }; // end of enum class Type

public:
  Joint(const JointConfig& config);
  ~Joint();

  /**
   * Update all the anchor points.
   */
  void updateAnchorPoints();

  /**
   * Attach the joint to the bodies.
   */
  void attach();

  /**
   * Detach the joint from the bodies.
   */
  void detach();

  /**
   * Awake the bodies.
   */
  void awake();

  /**
   * Cleanup.
   */
  void dispose();

  /** Helper functions **/
  std::pair<Vec3, Vec3> getPosition() const;
  Mat33 getMatrix() const;

public:
  // joint name
  std::string name;
  // The type of the joint.
  Type type;
  //  The previous joint in the world.
  Joint* prev;
  // The next joint in the world.
  Joint* next;
  RigidBody *body1, *body2;
  // The anchor point on the first rigid body in local coordinate system.
  Vec3 localAnchorPoint1;
  // The anchor point on the second rigid body in local coordinate system.
  Vec3 localAnchorPoint2;
  // The anchor point on the first rigid body in world coordinate system
  // relative to the body's origin.
  Vec3 relativeAnchorPoint1;
  // The anchor point on the second rigid body in world coordinate system
  // relative to the body's origin.
  Vec3 relativeAnchorPoint2;
  //  The anchor point on the first rigid body in world coordinate system.
  Vec3 anchorPoint1;
  // The anchor point on the second rigid body in world coordinate system.
  Vec3 anchorPoint2;
  //  Whether allow collision between connected rigid bodies or not.
  bool allowCollision;
  JointLink *b1Link, *b2Link;
  Mat44 matrix;

}; // end of class Joint

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_JOINT_JOINT_JOINT_H
