#include <oimo/collision/shape/tetra_shape.h>

#include <oimo/collision/broadphase/aabb.h>
#include <oimo/collision/broadphase/proxy.h>
#include <oimo/collision/shape/mass_info.h>
#include <oimo/collision/shape/shape_config.h>

namespace OIMO {

TetraShape::TetraShape(const ShapeConfig& config, const Vertex& p1,
                       const Vertex& p2, const Vertex& p3, const Vertex& p4)
    : Shape{config}
{
  type = Shape::Type::SHAPE_TETRA;
  // Vertices and faces of tetra
  verts = {{p1, p2, p3, p4}};
  faces = {{
    TetraShape::Mtri(0, 1, 2), //
    TetraShape::Mtri(1, 2, 3), //
    TetraShape::Mtri(2, 3, 4), //
    TetraShape::Mtri(4, 0, 1), //
  }};
}

TetraShape::~TetraShape()
{
}

void TetraShape::calculateMassInfo(MassInfo& /*out*/)
{
  // I guess you could calculate box mass and split it
  // in half for the tetra...
}
void TetraShape::updateProxy()
{
  aabb->setFromPoints(verts);
  aabb->expandByScalar(AABB::AABB_PROX);

  if (proxy != nullptr) {
    proxy->update();
  }
}

Face TetraShape::Mtri(float a, float b, float c)
{
  return Face(a, b, c);
}

} // end of namespace OIMO
