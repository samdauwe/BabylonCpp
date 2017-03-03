#ifndef OIMO_COLLISION_SHAPE_PARTICLE_SHAPE_H
#define OIMO_COLLISION_SHAPE_PARTICLE_SHAPE_H

#include <oimo/collision/shape/shape.h>

namespace OIMO {

struct MassInfo;
struct ShapeConfig;

/**
 * @brief A particule shape.
 */
class ParticleShape : public Shape {

public:
  ParticleShape(const ShapeConfig& config, const Vec3& normal);
  ~ParticleShape();

  void calculateMassInfo(MassInfo& out) override;
  void updateProxy() override;

public:
  Vec3 normal;

}; // end of class ParticleShape

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_SHAPE_PARTICLE_SHAPE_H
