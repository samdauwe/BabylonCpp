#include <babylon/physics/joint/hinge_joint.h>

#include <babylon/physics/iphysics_engine_plugin.h>
#include <babylon/physics/joint/physics_joint_data.h>

namespace BABYLON {

HingeJoint::HingeJoint(const PhysicsJointData& iJointData)
    : MotorEnabledJoint(PhysicsJoint::HingeJoint, iJointData)
{
}

HingeJoint::~HingeJoint() = default;

void HingeJoint::setMotor(float force, float maxForce, unsigned int motorIndex)
{
  _physicsPlugin->setMotor(this, force, maxForce, motorIndex);
}

void HingeJoint::setLimit(float upperLimit, float lowerLimit,
                          unsigned int motorIndex)
{
  _physicsPlugin->setLimit(this, upperLimit, lowerLimit, motorIndex);
}

} // end of namespace BABYLON
