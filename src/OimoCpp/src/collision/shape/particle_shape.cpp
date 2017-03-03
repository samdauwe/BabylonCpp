#include <oimo/collision/shape/particle_shape.h>

#include <oimo/collision/broadphase/aabb.h>
#include <oimo/collision/broadphase/proxy.h>
#include <oimo/collision/shape/mass_info.h>

namespace OIMO {

ParticleShape::ParticleShape(const ShapeConfig& config, const Vec3& _normal)
    : Shape{config}, normal{_normal}
{
  type = Shape::Type::SHAPE_PARTICLE;
}

ParticleShape::~ParticleShape()
{
}

void ParticleShape::calculateMassInfo(MassInfo& out)
{
  float inertia = 0.f;
  out.inertia.set(inertia, 0.f, 0.f, 0.f, inertia, 0.f, 0.f, 0.f, inertia);
}

void ParticleShape::updateProxy()
{
  const float p = 0.f;

  aabb->set(position.x - p, position.x + p, //
            position.y - p, position.y + p, //
            position.z - p, position.z + p);

  if (proxy != nullptr) {
    proxy->update();
  }
}

} // end of namespace OIMO
