#include <oimo/collision/narrowphase/sphere_box_collision_detector.h>

#include <cmath>

#include <oimo/collision/shape/box_shape.h>
#include <oimo/collision/shape/sphere_shape.h>
#include <oimo/constraint/contact/contact_manifold.h>

namespace OIMO {

SphereBoxCollisionDetector::SphereBoxCollisionDetector(bool _flip)
    : CollisionDetector{}
{
  flip = _flip;
}

SphereBoxCollisionDetector::~SphereBoxCollisionDetector()
{
}

void SphereBoxCollisionDetector::detectCollision(Shape* shape1, Shape* shape2,
                                                 ContactManifold* manifold)
{
  Shape *_s, *_b;
  if (flip) {
    _s = shape2;
    _b = shape1;
  }
  else {
    _s = shape1;
    _b = shape2;
  }

  if (_s->type != Shape::Type::SHAPE_SPHERE
      || _b->type != Shape::Type::SHAPE_BOX) {
    return;
  }
  SphereShape* s = dynamic_cast<SphereShape*>(_s);
  BoxShape* b    = dynamic_cast<BoxShape*>(_b);

  const std::array<float, 18>& D = b->dimensions;

  const Vec3& ps = s->position;
  float psx      = ps.x;
  float psy      = ps.y;
  float psz      = ps.z;
  const Vec3& pb = b->position;
  float pbx      = pb.x;
  float pby      = pb.y;
  float pbz      = pb.z;
  float rad      = s->radius;

  float hw = b->halfWidth;
  float hh = b->halfHeight;
  float hd = b->halfDepth;

  float dx = psx - pbx;
  float dy = psy - pby;
  float dz = psz - pbz;
  float sx = D[0] * dx + D[1] * dy + D[2] * dz;
  float sy = D[3] * dx + D[4] * dy + D[5] * dz;
  float sz = D[6] * dx + D[7] * dy + D[8] * dz;
  float cx;
  float cy;
  float cz;
  float len;
  float invLen;
  unsigned int overlap = 0;
  if (sx > hw) {
    sx = hw;
  }
  else if (sx < -hw) {
    sx = -hw;
  }
  else {
    overlap = 1;
  }
  if (sy > hh) {
    sy = hh;
  }
  else if (sy < -hh) {
    sy = -hh;
  }
  else {
    overlap |= 2;
  }
  if (sz > hd) {
    sz = hd;
  }
  else if (sz < -hd) {
    sz = -hd;
  }
  else {
    overlap |= 4;
  }
  if (overlap == 7) {
    // center of sphere is in the box
    if (sx < 0) {
      dx = hw + sx;
    }
    else {
      dx = hw - sx;
    }
    if (sy < 0) {
      dy = hh + sy;
    }
    else {
      dy = hh - sy;
    }
    if (sz < 0) {
      dz = hd + sz;
    }
    else {
      dz = hd - sz;
    }
    if (dx < dy) {
      if (dx < dz) {
        len = dx - hw;
        if (sx < 0) {
          sx = -hw;
          dx = D[0];
          dy = D[1];
          dz = D[2];
        }
        else {
          sx = hw;
          dx = -D[0];
          dy = -D[1];
          dz = -D[2];
        }
      }
      else {
        len = dz - hd;
        if (sz < 0) {
          sz = -hd;
          dx = D[6];
          dy = D[7];
          dz = D[8];
        }
        else {
          sz = hd;
          dx = -D[6];
          dy = -D[7];
          dz = -D[8];
        }
      }
    }
    else {
      if (dy < dz) {
        len = dy - hh;
        if (sy < 0) {
          sy = -hh;
          dx = D[3];
          dy = D[4];
          dz = D[5];
        }
        else {
          sy = hh;
          dx = -D[3];
          dy = -D[4];
          dz = -D[5];
        }
      }
      else {
        len = dz - hd;
        if (sz < 0) {
          sz = -hd;
          dx = D[6];
          dy = D[7];
          dz = D[8];
        }
        else {
          sz = hd;
          dx = -D[6];
          dy = -D[7];
          dz = -D[8];
        }
      }
    }
    cx = pbx + sx * D[0] + sy * D[3] + sz * D[6];
    cy = pby + sx * D[1] + sy * D[4] + sz * D[7];
    cz = pbz + sx * D[2] + sy * D[5] + sz * D[8];
    manifold->addPoint(psx + rad * dx, psy + rad * dy, psz + rad * dz, dx, dy,
                       dz, len - rad, flip);
  }
  else {
    cx  = pbx + sx * D[0] + sy * D[3] + sz * D[6];
    cy  = pby + sx * D[1] + sy * D[4] + sz * D[7];
    cz  = pbz + sx * D[2] + sy * D[5] + sz * D[8];
    dx  = cx - ps.x;
    dy  = cy - ps.y;
    dz  = cz - ps.z;
    len = dx * dx + dy * dy + dz * dz;
    if (len > 0 && len < rad * rad) {
      len    = std::sqrt(len);
      invLen = 1 / len;
      dx *= invLen;
      dy *= invLen;
      dz *= invLen;
      manifold->addPoint(psx + rad * dx, psy + rad * dy, psz + rad * dz, dx, dy,
                         dz, len - rad, flip);
    }
  }
}

} // end of namespace OIMO
