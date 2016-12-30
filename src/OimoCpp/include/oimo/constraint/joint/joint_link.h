#ifndef OIMO_CONSTRAINT_JOINT_JOINT_LINK_H
#define OIMO_CONSTRAINT_JOINT_JOINT_LINK_H

namespace OIMO {

class Joint;
class RigidBody;

/**
 * @brief A link list of joints.
 */
struct JointLink {
  JointLink(Joint* _joint) : joint{_joint}
  {
  }
  // The previous joint link.
  JointLink* prev = nullptr;
  // The next joint link.
  JointLink* next = nullptr;
  // The other rigid body connected to the joint.
  RigidBody* body = nullptr;
  // The joint of the link.
  Joint* joint;
}; // end of struct JointLink

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_JOINT_JOINT_LINK_H
