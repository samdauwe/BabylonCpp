#ifndef OIMO_COLLISION_SHAPE_TETRA_SHAPE_H
#define OIMO_COLLISION_SHAPE_TETRA_SHAPE_H

#include <array>

#include <oimo/collision/shape/shape.h>
#include <oimo/shape/face.h>
#include <oimo/shape/vertex.h>

namespace OIMO {

struct MassInfo;
struct ShapeConfig;

/**
 * @brief A tetra shape.
 */
class TetraShape : public Shape {

public:
  TetraShape(const ShapeConfig& config, const Vertex& p1, const Vertex& p2,
             const Vertex& p3, const Vertex& p4);
  ~TetraShape();

  void calculateMassInfo(MassInfo& out) override;
  void updateProxy() override;

  static Face Mtri(unsigned int a, unsigned int b, unsigned int c);

public:
  // Vertices and faces of tetra
  std::array<Vertex, 4> verts;
  std::array<Face, 4> faces;

}; // end of class TetraShape

} // end of namespace OIMO

#endif // end of OIMO_COLLISION_SHAPE_TETRA_SHAPE_H
