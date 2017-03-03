#include <oimo/collision/shape/box_shape.h>

#include <oimo/collision/broadphase/aabb.h>
#include <oimo/collision/broadphase/proxy.h>
#include <oimo/collision/shape/mass_info.h>

namespace OIMO {

BoxShape::BoxShape(const ShapeConfig& config, float _width, float _height,
                   float _depth)
    : Shape{config}
    , width{_width}
    , height{_height}
    , depth{_depth}
    , halfWidth{width * 0.5f}
    , halfHeight{height * 0.5f}
    , halfDepth{depth * 0.5f}
{
  type = Shape::Type::SHAPE_BOX;
}

BoxShape::~BoxShape()
{
}

void BoxShape::calculateMassInfo(MassInfo& out)
{
  const float mass  = width * height * depth * density;
  const float divid = 1.f / 12.f;
  out.mass          = mass;
  out.inertia.set(mass * (height * height + depth * depth) * divid, 0.f, 0.f,
                  0.f, mass * (width * width + depth * depth) * divid, 0.f, 0.f,
                  0.f, mass * (width * width + height * height) * divid);
}

void BoxShape::updateProxy()
{
  const std::array<float, 9>& te = rotation.elements;

  // Width
  dimensions[0] = te[0];
  dimensions[1] = te[3];
  dimensions[2] = te[6];
  // Height
  dimensions[3] = te[1];
  dimensions[4] = te[4];
  dimensions[5] = te[7];
  // Depth
  dimensions[6] = te[2];
  dimensions[7] = te[5];
  dimensions[8] = te[8];
  // halp Width
  dimensions[9]  = te[0] * halfWidth;
  dimensions[10] = te[3] * halfWidth;
  dimensions[11] = te[6] * halfWidth;
  // halp Height
  dimensions[12] = te[1] * halfHeight;
  dimensions[13] = te[4] * halfHeight;
  dimensions[14] = te[7] * halfHeight;
  // halp Depth
  dimensions[15] = te[2] * halfDepth;
  dimensions[16] = te[5] * halfDepth;
  dimensions[17] = te[8] * halfDepth;

  float wx = dimensions[9];
  float wy = dimensions[10];
  float wz = dimensions[11];
  float hx = dimensions[12];
  float hy = dimensions[13];
  float hz = dimensions[14];
  float dx = dimensions[15];
  float dy = dimensions[16];
  float dz = dimensions[17];

  float x = position.x;
  float y = position.y;
  float z = position.z;

  // v1
  elements[0] = x + wx + hx + dx;
  elements[1] = y + wy + hy + dy;
  elements[2] = z + wz + hz + dz;
  // v2
  elements[3] = x + wx + hx - dx;
  elements[4] = y + wy + hy - dy;
  elements[5] = z + wz + hz - dz;
  // v3
  elements[6] = x + wx - hx + dx;
  elements[7] = y + wy - hy + dy;
  elements[8] = z + wz - hz + dz;
  // v4
  elements[9]  = x + wx - hx - dx;
  elements[10] = y + wy - hy - dy;
  elements[11] = z + wz - hz - dz;
  // v5
  elements[12] = x - wx + hx + dx;
  elements[13] = y - wy + hy + dy;
  elements[14] = z - wz + hz + dz;
  // v6
  elements[15] = x - wx + hx - dx;
  elements[16] = y - wy + hy - dy;
  elements[17] = z - wz + hz - dz;
  // v7
  elements[18] = x - wx - hx + dx;
  elements[19] = y - wy - hy + dy;
  elements[20] = z - wz - hz + dz;
  // v8
  elements[21] = x - wx - hx - dx;
  elements[22] = y - wy - hy - dy;
  elements[23] = z - wz - hz - dz;

  float w = dimensions[9] < 0 ? -dimensions[9] : dimensions[9];
  float h = dimensions[10] < 0 ? -dimensions[10] : dimensions[10];
  float d = dimensions[11] < 0 ? -dimensions[11] : dimensions[11];

  w = dimensions[12] < 0 ? w - dimensions[12] : w + dimensions[12];
  h = dimensions[13] < 0 ? h - dimensions[13] : h + dimensions[13];
  d = dimensions[14] < 0 ? d - dimensions[14] : d + dimensions[14];

  w = dimensions[15] < 0 ? w - dimensions[15] : w + dimensions[15];
  h = dimensions[16] < 0 ? h - dimensions[16] : h + dimensions[16];
  d = dimensions[17] < 0 ? d - dimensions[17] : d + dimensions[17];

  const float p = AABB::AABB_PROX;

  aabb->set(position.x - w - p, position.x + w + p, //
            position.y - h - p, position.y + h + p, //
            position.z - d - p, position.z + d + p);

  if (proxy != nullptr) {
    proxy->update();
  }
}

} // end of namespace OIMO
