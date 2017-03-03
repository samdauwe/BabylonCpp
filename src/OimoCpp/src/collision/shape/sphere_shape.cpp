#include <oimo/collision/shape/sphere_shape.h>

#include <oimo/collision/broadphase/aabb.h>
#include <oimo/collision/broadphase/proxy.h>
#include <oimo/collision/shape/mass_info.h>
#include <oimo/oimo_constants.h>

namespace OIMO {

SphereShape::SphereShape(const ShapeConfig& config, float _radius)
    : Shape{config}, radius{_radius}
{
  type = Shape::Type::SHAPE_SPHERE;
}

SphereShape::~SphereShape()
{
}

float SphereShape::volume() const
{
  return Math::PI * radius * 1.333333f;
}

void SphereShape::calculateMassInfo(MassInfo& out)
{
  float mass    = volume() * radius * radius * density;
  out.mass      = mass;
  float inertia = mass * radius * radius * 0.4f;
  out.inertia.set(inertia, 0.f, 0.f, 0.f, inertia, 0.f, 0.f, 0.f, inertia);
}

void SphereShape::updateProxy()
{
  const float p = AABB::AABB_PROX;

  aabb->set(position.x - radius - p, position.x + radius + p, //
            position.y - radius - p, position.y + radius + p, //
            position.z - radius - p, position.z + radius + p);

  if (proxy != nullptr) {
    proxy->update();
  }
}

} // end of namespace OIMO
