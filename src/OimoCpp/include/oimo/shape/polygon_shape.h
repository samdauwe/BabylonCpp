#ifndef OIMO_SHAPE_POLYGON_SHAPE_H
#define OIMO_SHAPE_POLYGON_SHAPE_H

#include <vector>

#include <oimo/collision/shape/shape.h>
#include <oimo/shape/face.h>
#include <oimo/shape/vertex.h>

namespace OIMO {

struct ShapeConfig;

/**
 * @brief A polygon shape. Calculated with vertices and faces.
 */
class PolygonShape : public Shape {

public:
  PolygonShape(const ShapeConfig& config, const std::vector<Vertex>& verts,
               const std::vector<Face>& faces);
  ~PolygonShape();

  void updateProxy() override;

private:
  std::vector<Vertex> _vertices;
  const std::vector<Face> _faces;

}; // end of class PolygonShape

} // end of namespace OIMO

#endif // end of OIMO_SHAPE_POLYGON_SHAPE_H
