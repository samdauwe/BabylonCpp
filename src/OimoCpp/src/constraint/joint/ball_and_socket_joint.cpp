#include <oimo/constraint/joint/ball_and_socket_joint.h>

#include <oimo/constraint/joint/base/linear_constraint.h>

namespace OIMO {

BallAndSocketJoint::BallAndSocketJoint(const JointConfig& config)
    : Joint{config}
    , _lc{new LinearConstraint(this)}
{
  // Set joint type
  type = Joint::Type::JOINT_BALL_AND_SOCKET;
}

BallAndSocketJoint::~BallAndSocketJoint()
{
}

void BallAndSocketJoint::preSolve(float timeStep, float invTimeStep)
{
  updateAnchorPoints();
  _lc->preSolve(timeStep, invTimeStep);
}

void BallAndSocketJoint::solve()
{
  _lc->solve();
}

void BallAndSocketJoint::postSolve()
{
}

} // end of namespace OIMO
