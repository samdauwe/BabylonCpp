#ifndef OIMO_COLLISION_SHAPE_SPHERE_SHAPE_H
#define OIMO_COLLISION_SHAPE_SPHERE_SHAPE_H

#include <oimo/collision/shape/shape.h>

namespace OIMO {

struct MassInfo;
struct ShapeConfig;

/**
 * @brief A sphere shape.
 */
class SphereShape : public Shape {

public:
  SphereShape(const ShapeConfig& config, float radius);
  ~SphereShape();

  void calculateMassInfo(MassInfo& out) override;
  void updateProxy() override;

public:
  // The radius of the shape.
  float radius;

}; // end of class SphereShape

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_SHAPE_SPHERE_SHAPE_H
