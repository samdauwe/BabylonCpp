#include <oimo/constraint/joint/ball_and_socket_joint.h>

#include <oimo/constraint/joint/base/linear_constraint.h>

namespace OIMO {

BallAndSocketJoint::BallAndSocketJoint(const JointConfig& config)
    : Joint{config}
{
  // Set joint type
  type = Joint::Type::JOINT_BALL_AND_SOCKET;

  _lc = make_unique<LinearConstraint>(this);
}

BallAndSocketJoint::~BallAndSocketJoint()
{
}

void BallAndSocketJoint::preSolve(float timeStep, float invTimeStep)
{
  updateAnchorPoints();

  // preSolve

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
