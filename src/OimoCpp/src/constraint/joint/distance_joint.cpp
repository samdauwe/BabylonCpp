#include <oimo/constraint/joint/distance_joint.h>

#include <oimo/constraint/joint/base/translational_constraint.h>
#include <oimo/constraint/joint/limit_motor.h>

namespace OIMO {

DistanceJoint::DistanceJoint(const JointConfig& config, float minDistance,
                             float maxDistance)
    : Joint{config}
{
  // Set joint type
  type = Joint::Type::JOINT_DISTANCE;
  // The limit and motor information of the joint.
  _limitMotor             = make_unique<LimitMotor>(_nor, true);
  _limitMotor->lowerLimit = minDistance;
  _limitMotor->upperLimit = maxDistance;
  _t = make_unique<TranslationalConstraint>(this, _limitMotor.get());
}

DistanceJoint::~DistanceJoint()
{
}

void DistanceJoint::preSolve(float timeStep, float invTimeStep)
{
  updateAnchorPoints();

  _nor.sub(anchorPoint2, anchorPoint1).normalize();

  // preSolve

  _t->preSolve(timeStep, invTimeStep);
}

void DistanceJoint::solve()
{
  _t->solve();
}

void DistanceJoint::postSolve()
{
}

} // end of namespace OIMO
