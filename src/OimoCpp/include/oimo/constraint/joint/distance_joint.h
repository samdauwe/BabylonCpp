#ifndef OIMO_CONSTRAINT_JOINT_DISTANCE_JOINT_H
#define OIMO_CONSTRAINT_JOINT_DISTANCE_JOINT_H

#include <oimo/constraint/joint/joint.h>
#include <oimo/constraint/joint/joint_config.h>
#include <oimo/math/vec3.h>

namespace OIMO {

class LimitMotor;
class TranslationalConstraint;

/**
 * @brief A distance joint limits the distance between two anchor points on
 * rigid bodies.
 */
class DistanceJoint : public Joint {

public:
  DistanceJoint(const JointConfig& config, float minDistance,
                float maxDistance);
  ~DistanceJoint();

  void preSolve(float timeStep, float invTimeStep) override;
  void solve() override;
  void postSolve() override;

private:
  Vec3 _normal;
  Vec3 _nr;
  TranslationalConstraint* _t;
  LimitMotor* _limitMotor;

}; // end of class DistanceJoint

} // end of namespace OIMO

#endif // end of OIMO_CONSTRAINT_JOINT_DISTANCE_JOINT_H
