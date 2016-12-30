#ifndef OIMO_COLLISION_SHAPE_BOX_SHAPE_H
#define OIMO_COLLISION_SHAPE_BOX_SHAPE_H

#include <oimo/collision/shape/shape.h>

#include <array>

namespace OIMO {

struct MassInfo;
struct ShapeConfig;

/**
 * @brief A box shape.
 */
class BoxShape : public Shape {

public:
  BoxShape(const ShapeConfig& config, float width, float height, float depth);
  ~BoxShape();

  void calculateMassInfo(MassInfo& out) override;
  void updateProxy() override;

public:
  // The width of the box.
  float width;
  // The height of the box.
  float height;
  // The depth of the box.
  float depth;
  // The half-width of the box.
  float halfWidth;
  // The half-height of the box.
  float halfHeight;
  // The half-depth of the box.
  float halfDepth;
  std::array<float, 18> dimensions;
  std::array<float, 24> elements;

}; // end of class BoxShape

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_SHAPE_BOX_SHAPE_H
