#include <babylon/physics/joint/distance_joint.h>

#include <babylon/physics/iphysics_engine_plugin.h>
#include <babylon/physics/joint/distance_joint_data.h>

namespace BABYLON {

DistanceJoint::DistanceJoint(const DistanceJointData& iJointData)
    : PhysicsJoint(PhysicsJoint::DistanceJoint, iJointData)
{
}

DistanceJoint::~DistanceJoint() = default;

void DistanceJoint::updateDistance(float maxDistance, float minDistance)
{
  _physicsPlugin->updateDistanceJoint(this, maxDistance, minDistance);
}

} // end of namespace BABYLON
