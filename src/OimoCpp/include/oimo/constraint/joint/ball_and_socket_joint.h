#ifndef OIMO_CONSTRAINT_JOINT_BALL_AND_SOCKET_JOINT_H
#define OIMO_CONSTRAINT_JOINT_BALL_AND_SOCKET_JOINT_H

#include <oimo/constraint/joint/joint.h>
#include <oimo/constraint/joint/joint_config.h>
#include <oimo/oimo_utils.h>

namespace OIMO {

class LinearConstraint;

/**
 * @brief A ball-and-socket joint limits relative translation on two anchor
 * points on rigid bodies.
 */
class BallAndSocketJoint : public Joint {

public:
  BallAndSocketJoint(const JointConfig& config);
  ~BallAndSocketJoint();

  void preSolve(float timeStep, float invTimeStep) override;
  void solve() override;
  void postSolve() override;

private:
  std::unique_ptr<LinearConstraint> _lc;

}; // end of class BallAndSocketJoint

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_JOINT_BALL_AND_SOCKET_JOINT_H
