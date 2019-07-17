#include <babylon/physics/joint/motor_enabled_joint.h>

#include <babylon/physics/iphysics_engine_plugin.h>
#include <babylon/physics/joint/physics_joint_data.h>

namespace BABYLON {

MotorEnabledJoint::MotorEnabledJoint(unsigned int iJointType,
                                     const PhysicsJointData& iJointData)
    : PhysicsJoint(iJointType, iJointData)
{
}

MotorEnabledJoint::~MotorEnabledJoint()
{
}

void MotorEnabledJoint::setMotor(float force, float maxForce,
                                 unsigned int motorIndex)
{
  _physicsPlugin->setMotor(this, force, maxForce, motorIndex);
}

void MotorEnabledJoint::setLimit(float upperLimit, float lowerLimit,
                                 unsigned int motorIndex)
{
  _physicsPlugin->setLimit(this, upperLimit, lowerLimit, motorIndex);
}

} // end of namespace BABYLON
