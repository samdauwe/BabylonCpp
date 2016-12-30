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
  _limitMotor             = new LimitMotor(_normal, true);
  _limitMotor->lowerLimit = minDistance;
  _limitMotor->upperLimit = maxDistance;
  _t                      = new TranslationalConstraint(this, _limitMotor);
}

DistanceJoint::~DistanceJoint()
{
}

void DistanceJoint::preSolve(float timeStep, float invTimeStep)
{
  updateAnchorPoints();
  _nr.sub(anchorPoint2, anchorPoint1);
  _normal.normalize(_nr);
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
