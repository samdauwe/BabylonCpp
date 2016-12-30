#include <babylon/physics/joint/physics_joint.h>

#include <babylon/physics/iphysics_engine_plugin.h>

namespace BABYLON {

PhysicsJoint::PhysicsJoint(unsigned int jointType,
                           const PhysicsJointData& jointData)
    : _jointType{jointType}, _jointData{jointData}
{
}

PhysicsJoint::~PhysicsJoint()
{
}

PhysicsJoint* PhysicsJoint::physicsJoint()
{
  return _physicsJoint;
}

void PhysicsJoint::setPhysicsJoint(PhysicsJoint* newJoint)
{
  if (_physicsJoint) {
    // remove from the wolrd
  }

  _physicsJoint = newJoint;
}

void PhysicsJoint::setPhysicsPlugin(IPhysicsEnginePlugin* physicsPlugin)
{
  _physicsPlugin = physicsPlugin;
}

void PhysicsJoint::executeNativeFunction(
  const std::function<void(IWorld* world, PhysicsJoint* physicsJoint)>& func)
{
  func(_physicsPlugin->world, _physicsJoint);
}

} // end of namespace BABYLON
