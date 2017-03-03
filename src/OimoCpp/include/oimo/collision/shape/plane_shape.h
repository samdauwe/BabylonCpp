#ifndef OIMO_COLLISION_SHAPE_PLANE_SHAPE_H
#define OIMO_COLLISION_SHAPE_PLANE_SHAPE_H

#include <oimo/collision/shape/shape.h>

namespace OIMO {

struct MassInfo;
struct ShapeConfig;

/**
 * @brief A plane shape.
 */
class PlaneShape : public Shape {

public:
  PlaneShape(const ShapeConfig& config,
             const Vec3& normal = Vec3(0.f, 1.f, 0.f));
  ~PlaneShape();

  void calculateMassInfo(MassInfo& out) override;
  void updateProxy() override;

public:
  // radius of the shape.
  Vec3 normal;

}; // end of class PlaneShape

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_SHAPE_PLANE_SHAPE_H
