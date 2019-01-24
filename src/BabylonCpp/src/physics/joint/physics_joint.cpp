#include <babylon/physics/joint/physics_joint.h>

#include <babylon/physics/iphysics_engine_plugin.h>

namespace BABYLON {

PhysicsJoint::PhysicsJoint(unsigned int iJointType,
                           const PhysicsJointData& iJointData)
    : jointType{iJointType}
    , jointData{iJointData}
    , physicsJoint{this, &PhysicsJoint::get_physicsJoint,
                   &PhysicsJoint::set_physicsJoint}
    , physicsPlugin{this, &PhysicsJoint::set_physicsPlugin}
{
}

PhysicsJoint::~PhysicsJoint()
{
}

PhysicsJoint*& PhysicsJoint::get_physicsJoint()
{
  return _physicsJoint;
}

void PhysicsJoint::set_physicsJoint(PhysicsJoint* const& newJoint)
{
  if (_physicsJoint) {
    // remove from the wolrd
  }

  _physicsJoint = newJoint;
}

void PhysicsJoint::set_physicsPlugin(IPhysicsEnginePlugin* const& physicsPlugin)
{
  _physicsPlugin = physicsPlugin;
}

void PhysicsJoint::executeNativeFunction(
  const std::function<void(Mesh* world, PhysicsJoint* physicsJoint)>& func)
{
  func(_physicsPlugin->world, _physicsJoint);
}

} // end of namespace BABYLON
