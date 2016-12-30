#ifndef OIMO_COLLISION_SHAPE_CYLINDER_SHAPE_H
#define OIMO_COLLISION_SHAPE_CYLINDER_SHAPE_H

#include <oimo/collision/shape/shape.h>

namespace OIMO {

struct MassInfo;
struct ShapeConfig;

/**
 * @brief A cylinder shape.
 */
class CylinderShape : public Shape {

public:
  CylinderShape(const ShapeConfig& config, float radius, float height);
  ~CylinderShape();

  void calculateMassInfo(MassInfo& out) override;
  void updateProxy() override;

public:
  float radius;
  float height;
  float halfHeight;
  Vec3 normalDirection;
  Vec3 halfDirection;

}; // end of class CylinderShape

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_SHAPE_CYLINDER_SHAPE_H
