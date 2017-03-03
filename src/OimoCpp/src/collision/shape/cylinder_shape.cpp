#include <oimo/collision/shape/cylinder_shape.h>

#include <cmath>

#include <oimo/collision/broadphase/aabb.h>
#include <oimo/collision/broadphase/proxy.h>
#include <oimo/collision/shape/mass_info.h>
#include <oimo/oimo_constants.h>

namespace OIMO {

CylinderShape::CylinderShape(const ShapeConfig& config, float _radius,
                             float _height)
    : Shape{config}, radius{_radius}, height{_height}, halfHeight{height * 0.5f}
{
  type = Shape::Type::SHAPE_CYLINDER;
}

CylinderShape::~CylinderShape()
{
}

void CylinderShape::calculateMassInfo(MassInfo& out)
{
  float rsq       = radius * radius;
  float mass      = Math::PI * rsq * height * density;
  float inertiaXZ = ((0.25f * rsq) + (0.0833f * height * height)) * mass;
  float inertiaY  = 0.5f * rsq;
  out.mass        = mass;
  out.inertia.set(inertiaXZ, 0.f, 0.f, 0.f, inertiaY, 0.f, 0.f, 0.f, inertiaXZ);
}

void CylinderShape::updateProxy()
{
  const std::array<float, 9>& te = rotation.elements;
  float len, wx, hy, dz, xx, yy, zz, w, h, d;

  xx = te[1] * te[1];
  yy = te[4] * te[4];
  zz = te[7] * te[7];

  normalDirection.set(te[1], te[4], te[7]);
  halfDirection.scale(normalDirection, halfHeight);

  wx  = 1.f - xx;
  len = std::sqrt(wx * wx + xx * yy + xx * zz);
  if (len > 0.f) {
    len = radius / len;
  }
  wx *= len;
  hy  = 1.f - yy;
  len = std::sqrt(yy * xx + hy * hy + yy * zz);
  if (len > 0.f) {
    len = radius / len;
  }
  hy *= len;
  dz  = 1.f - zz;
  len = std::sqrt(zz * xx + zz * yy + dz * dz);
  if (len > 0.f) {
    len = radius / len;
  }
  dz *= len;

  w = halfDirection.x < 0.f ? -halfDirection.x : halfDirection.x;
  h = halfDirection.y < 0.f ? -halfDirection.y : halfDirection.y;
  d = halfDirection.z < 0.f ? -halfDirection.z : halfDirection.z;

  w = wx < 0.f ? w - wx : w + wx;
  h = hy < 0.f ? h - hy : h + hy;
  d = dz < 0.f ? d - dz : d + dz;

  const float p = AABB::AABB_PROX;

  aabb->set(position.x - w - p, position.x + w + p, //
            position.y - h - p, position.y + h + p, //
            position.z - d - p, position.z + d + p);

  if (proxy != nullptr) {
    proxy->update();
  }
}

} // end of namespace OIMO
