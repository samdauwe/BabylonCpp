#include <oimo/collision/shape/plane_shape.h>

#include <oimo/collision/broadphase/aabb.h>
#include <oimo/collision/broadphase/proxy.h>
#include <oimo/collision/shape/mass_info.h>
#include <oimo/oimo_utils.h>

namespace OIMO {

PlaneShape::PlaneShape(const ShapeConfig& config, const Vec3& _normal)
    : Shape{config}, normal{_normal}
{
  type = Shape::Type::SHAPE_PLANE;
}

PlaneShape::~PlaneShape()
{
}

void PlaneShape::calculateMassInfo(MassInfo& out)
{
  out.mass      = density;
  float inertia = 1.f;
  out.inertia.set(inertia, 0.f, 0.f, 0.f, inertia, 0.f, 0.f, 0.f, inertia);
}

void PlaneShape::updateProxy()
{
  const float p   = AABB::AABB_PROX;
  const float min = -std::numeric_limits<float>::infinity();
  const float max = std::numeric_limits<float>::infinity();
  const Vec3& n   = normal;
  // The plane AABB is infinite, except if the normal is pointing along any axis
  aabb->set(floats_are_equal(n.x, -1.f) ? position.x - p : min,
            floats_are_equal(n.x, 1) ? position.x + p : max,
            floats_are_equal(n.y, -1) ? position.y - p : min,
            floats_are_equal(n.y, 1) ? position.y + p : max,
            floats_are_equal(n.z, -1) ? position.z - p : min,
            floats_are_equal(n.z, 1) ? position.z + p : max);

  if (proxy != nullptr) {
    proxy->update();
  }
}

} // end of namespace OIMO
