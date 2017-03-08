#include <oimo/collision/shape/tetra_shape.h>

#include <oimo/collision/broadphase/aabb.h>
#include <oimo/collision/broadphase/proxy.h>
#include <oimo/collision/shape/mass_info.h>

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

void TetraShape::calculateMassInfo(MassInfo& out)
{
  // I guess you could calculate box mass and split it
  // in half for the tetra...
  aabb->setFromPoints(verts);
  const auto& p = aabb->elements;
  float x       = p[3] - p[0];
  float y       = p[4] - p[1];
  float z       = p[5] - p[2];
  float mass    = x * y * z * density;
  float divid   = 1.f / 12.f;
  out.mass      = mass;
  out.inertia.set(
    mass * (2.f * y * 2.f * y + 2.f * z * 2.f * z) * divid, 0.f, 0.f, //
    0.f, mass * (2.f * x * 2.f * x + 2.f * z * 2.f * z) * divid, 0.f, //
    0.f, 0.f, mass * (2.f * y * 2.f * y + 2.f * x * 2.f * x) * divid);
}

void TetraShape::updateProxy()
{
  aabb->setFromPoints(verts);
  aabb->expandByScalar(AABB::AABB_PROX);

  if (proxy != nullptr) {
    proxy->update();
  }
}

Face TetraShape::Mtri(unsigned int a, unsigned int b, unsigned int c)
{
  return Face(a, b, c);
}

} // end of namespace OIMO
