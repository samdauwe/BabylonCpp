#include <babylon/physics/joint/hinge_2_joint.h>

#include <babylon/physics/iphysics_engine_plugin.h>
#include <babylon/physics/joint/physics_joint_data.h>

namespace BABYLON {

Hinge2Joint::Hinge2Joint(const PhysicsJointData& iJointData)
    : MotorEnabledJoint(PhysicsJoint::Hinge2Joint, iJointData)
{
}

Hinge2Joint::~Hinge2Joint() = default;

void Hinge2Joint::setMotor(float targetSpeed, float maxForce,
                           unsigned int motorIndex)
{
  _physicsPlugin->setMotor(this, targetSpeed, maxForce, motorIndex);
}

void Hinge2Joint::setLimit(float upperLimit, float lowerLimit,
                           unsigned int motorIndex)
{
  _physicsPlugin->setLimit(this, upperLimit, lowerLimit, motorIndex);
}

} // end of namespace BABYLON
