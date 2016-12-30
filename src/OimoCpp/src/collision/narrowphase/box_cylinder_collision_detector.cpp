#include <oimo/collision/narrowphase/box_cylinder_collision_detector.h>

#include <cmath>

#include <oimo/collision/shape/box_shape.h>
#include <oimo/collision/shape/cylinder_shape.h>
#include <oimo/constraint/contact/contact_manifold.h>
#include <oimo/math/vec3.h>
#include <oimo/oimo_utils.h>

namespace OIMO {

BoxCylinderCollisionDetector::BoxCylinderCollisionDetector(bool _flip)
    : CollisionDetector{}
{
  flip = _flip;
}

BoxCylinderCollisionDetector::~BoxCylinderCollisionDetector()
{
}

bool BoxCylinderCollisionDetector::getSep(BoxShape* b, CylinderShape* c,
                                          Vec3& sep, Vec3& pos, Vec3& dep)
{
  float t1x, t1y, t1z;
  float t2x, t2y, t2z;
  Vec3 sup;
  float len;
  float p1x = 0.f, p1y = 0.f, p1z = 0.f;
  float p2x = 0.f, p2y = 0.f, p2z = 0.f;
  float v01x = b->position.x;
  float v01y = b->position.y;
  float v01z = b->position.z;
  float v02x = c->position.x;
  float v02y = c->position.y;
  float v02z = c->position.z;
  float v0x  = v02x - v01x;
  float v0y  = v02y - v01y;
  float v0z  = v02z - v01z;
  if (floats_are_equal(v0x * v0x + v0y * v0y + v0z * v0z, 0)) {
    v0y = 0.001f;
  }
  float nx = -v0x;
  float ny = -v0y;
  float nz = -v0z;
  supportPointB(b, -nx, -ny, -nz, sup);
  float v11x = sup.x;
  float v11y = sup.y;
  float v11z = sup.z;
  supportPointC(c, nx, ny, nz, sup);
  float v12x = sup.x;
  float v12y = sup.y;
  float v12z = sup.z;
  float v1x  = v12x - v11x;
  float v1y  = v12y - v11y;
  float v1z  = v12z - v11z;
  if (v1x * nx + v1y * ny + v1z * nz <= 0.f) {
    return false;
  }
  nx = v1y * v0z - v1z * v0y;
  ny = v1z * v0x - v1x * v0z;
  nz = v1x * v0y - v1y * v0x;
  if (nx * nx + ny * ny + nz * nz == 0.f) {
    sep.init(v1x - v0x, v1y - v0y, v1z - v0z);
    sep.normalize(sep);
    pos.init((v11x + v12x) * 0.5f, (v11y + v12y) * 0.5f, (v11z + v12z) * 0.5f);
    return true;
  }
  supportPointB(b, -nx, -ny, -nz, sup);
  float v21x = sup.x;
  float v21y = sup.y;
  float v21z = sup.z;
  supportPointC(c, nx, ny, nz, sup);
  float v22x = sup.x;
  float v22y = sup.y;
  float v22z = sup.z;
  float v2x  = v22x - v21x;
  float v2y  = v22y - v21y;
  float v2z  = v22z - v21z;
  if (v2x * nx + v2y * ny + v2z * nz <= 0.f) {
    return false;
  }
  t1x = v1x - v0x;
  t1y = v1y - v0y;
  t1z = v1z - v0z;
  t2x = v2x - v0x;
  t2y = v2y - v0y;
  t2z = v2z - v0z;
  nx  = t1y * t2z - t1z * t2y;
  ny  = t1z * t2x - t1x * t2z;
  nz  = t1x * t2y - t1y * t2x;
  if (nx * v0x + ny * v0y + nz * v0z > 0.f) {
    t1x  = v1x;
    t1y  = v1y;
    t1z  = v1z;
    v1x  = v2x;
    v1y  = v2y;
    v1z  = v2z;
    v2x  = t1x;
    v2y  = t1y;
    v2z  = t1z;
    t1x  = v11x;
    t1y  = v11y;
    t1z  = v11z;
    v11x = v21x;
    v11y = v21y;
    v11z = v21z;
    v21x = t1x;
    v21y = t1y;
    v21z = t1z;
    t1x  = v12x;
    t1y  = v12y;
    t1z  = v12z;
    v12x = v22x;
    v12y = v22y;
    v12z = v22z;
    v22x = t1x;
    v22y = t1y;
    v22z = t1z;
    nx   = -nx;
    ny   = -ny;
    nz   = -nz;
  }
  unsigned int iterations = 0;
  float v31x, v31y, v31z;
  float v32x, v32y, v32z;
  float v3x, v3y, v3z;
  bool hit = false;
  float b0, b1, b2, b3, sum, inv, separation;
  float v41x, v41y, v41z;
  float v42x, v42y, v42z;
  float v4x, v4y, v4z;
  while (true) {
    if (++iterations > 100) {
      return false;
    }
    supportPointB(b, -nx, -ny, -nz, sup);
    v31x = sup.x;
    v31y = sup.y;
    v31z = sup.z;
    supportPointC(c, nx, ny, nz, sup);
    v32x = sup.x;
    v32y = sup.y;
    v32z = sup.z;
    v3x  = v32x - v31x;
    v3y  = v32y - v31y;
    v3z  = v32z - v31z;
    if (v3x * nx + v3y * ny + v3z * nz <= 0.f) {
      return false;
    }
    if ((v1y * v3z - v1z * v3y) * v0x + (v1z * v3x - v1x * v3z) * v0y
          + (v1x * v3y - v1y * v3x) * v0z
        < 0.f) {
      v2x  = v3x;
      v2y  = v3y;
      v2z  = v3z;
      v21x = v31x;
      v21y = v31y;
      v21z = v31z;
      v22x = v32x;
      v22y = v32y;
      v22z = v32z;
      t1x  = v1x - v0x;
      t1y  = v1y - v0y;
      t1z  = v1z - v0z;
      t2x  = v3x - v0x;
      t2y  = v3y - v0y;
      t2z  = v3z - v0z;
      nx   = t1y * t2z - t1z * t2y;
      ny   = t1z * t2x - t1x * t2z;
      nz   = t1x * t2y - t1y * t2x;
      continue;
    }
    if ((v3y * v2z - v3z * v2y) * v0x + (v3z * v2x - v3x * v2z) * v0y
          + (v3x * v2y - v3y * v2x) * v0z
        < 0.f) {
      v1x  = v3x;
      v1y  = v3y;
      v1z  = v3z;
      v11x = v31x;
      v11y = v31y;
      v11z = v31z;
      v12x = v32x;
      v12y = v32y;
      v12z = v32z;
      t1x  = v3x - v0x;
      t1y  = v3y - v0y;
      t1z  = v3z - v0z;
      t2x  = v2x - v0x;
      t2y  = v2y - v0y;
      t2z  = v2z - v0z;
      nx   = t1y * t2z - t1z * t2y;
      ny   = t1z * t2x - t1x * t2z;
      nz   = t1x * t2y - t1y * t2x;
      continue;
    }
    hit = false;
    while (true) {
      t1x = v2x - v1x;
      t1y = v2y - v1y;
      t1z = v2z - v1z;
      t2x = v3x - v1x;
      t2y = v3y - v1y;
      t2z = v3z - v1z;
      nx  = t1y * t2z - t1z * t2y;
      ny  = t1z * t2x - t1x * t2z;
      nz  = t1x * t2y - t1y * t2x;
      len = 1.f / std::sqrt(nx * nx + ny * ny + nz * nz);
      nx *= len;
      ny *= len;
      nz *= len;
      if (nx * v1x + ny * v1y + nz * v1z >= 0 && !hit) {
        b0 = (v1y * v2z - v1z * v2y) * v3x + (v1z * v2x - v1x * v2z) * v3y
             + (v1x * v2y - v1y * v2x) * v3z;
        b1 = (v3y * v2z - v3z * v2y) * v0x + (v3z * v2x - v3x * v2z) * v0y
             + (v3x * v2y - v3y * v2x) * v0z;
        b2 = (v0y * v1z - v0z * v1y) * v3x + (v0z * v1x - v0x * v1z) * v3y
             + (v0x * v1y - v0y * v1x) * v3z;
        b3 = (v2y * v1z - v2z * v1y) * v0x + (v2z * v1x - v2x * v1z) * v0y
             + (v2x * v1y - v2y * v1x) * v0z;
        sum = b0 + b1 + b2 + b3;
        if (sum <= 0.f) {
          b0 = 0.f;
          b1 = (v2y * v3z - v2z * v3y) * nx + (v2z * v3x - v2x * v3z) * ny
               + (v2x * v3y - v2y * v3x) * nz;
          b2 = (v3y * v2z - v3z * v2y) * nx + (v3z * v2x - v3x * v2z) * ny
               + (v3x * v2y - v3y * v2x) * nz;
          b3 = (v1y * v2z - v1z * v2y) * nx + (v1z * v2x - v1x * v2z) * ny
               + (v1x * v2y - v1y * v2x) * nz;
          sum = b1 + b2 + b3;
        }
        inv = 1.f / sum;
        p1x = (v01x * b0 + v11x * b1 + v21x * b2 + v31x * b3) * inv;
        p1y = (v01y * b0 + v11y * b1 + v21y * b2 + v31y * b3) * inv;
        p1z = (v01z * b0 + v11z * b1 + v21z * b2 + v31z * b3) * inv;
        p2x = (v02x * b0 + v12x * b1 + v22x * b2 + v32x * b3) * inv;
        p2y = (v02y * b0 + v12y * b1 + v22y * b2 + v32y * b3) * inv;
        p2z = (v02z * b0 + v12z * b1 + v22z * b2 + v32z * b3) * inv;
        hit = true;
      }
      supportPointB(b, -nx, -ny, -nz, sup);
      v41x = sup.x;
      v41y = sup.y;
      v41z = sup.z;
      supportPointC(c, nx, ny, nz, sup);
      v42x       = sup.x;
      v42y       = sup.y;
      v42z       = sup.z;
      v4x        = v42x - v41x;
      v4y        = v42y - v41y;
      v4z        = v42z - v41z;
      separation = -(v4x * nx + v4y * ny + v4z * nz);
      if ((v4x - v3x) * nx + (v4y - v3y) * ny + (v4z - v3z) * nz <= 0.01f
          || separation >= 0.f) {
        if (hit) {
          sep.init(-nx, -ny, -nz);
          pos.init((p1x + p2x) * 0.5f, (p1y + p2y) * 0.5f, (p1z + p2z) * 0.5f);
          dep.x = separation;
          return true;
        }
        return false;
      }
      if ((v4y * v1z - v4z * v1y) * v0x + (v4z * v1x - v4x * v1z) * v0y
            + (v4x * v1y - v4y * v1x) * v0z
          < 0.f) {
        if ((v4y * v2z - v4z * v2y) * v0x + (v4z * v2x - v4x * v2z) * v0y
              + (v4x * v2y - v4y * v2x) * v0z
            < 0.f) {
          v1x  = v4x;
          v1y  = v4y;
          v1z  = v4z;
          v11x = v41x;
          v11y = v41y;
          v11z = v41z;
          v12x = v42x;
          v12y = v42y;
          v12z = v42z;
        }
        else {
          v3x  = v4x;
          v3y  = v4y;
          v3z  = v4z;
          v31x = v41x;
          v31y = v41y;
          v31z = v41z;
          v32x = v42x;
          v32y = v42y;
          v32z = v42z;
        }
      }
      else {
        if ((v4y * v3z - v4z * v3y) * v0x + (v4z * v3x - v4x * v3z) * v0y
              + (v4x * v3y - v4y * v3x) * v0z
            < 0.f) {
          v2x  = v4x;
          v2y  = v4y;
          v2z  = v4z;
          v21x = v41x;
          v21y = v41y;
          v21z = v41z;
          v22x = v42x;
          v22y = v42y;
          v22z = v42z;
        }
        else {
          v1x  = v4x;
          v1y  = v4y;
          v1z  = v4z;
          v11x = v41x;
          v11y = v41y;
          v11z = v41z;
          v12x = v42x;
          v12y = v42y;
          v12z = v42z;
        }
      }
    }
  }
  // return false;
}

void BoxCylinderCollisionDetector::supportPointB(BoxShape* b, float dx,
                                                 float dy, float dz, Vec3& out)
{
  const std::array<float, 9>& rot = b->rotation.elements;
  float ldx = rot[0] * dx + rot[3] * dy + rot[6] * dz;
  float ldy = rot[1] * dx + rot[4] * dy + rot[7] * dz;
  float ldz = rot[2] * dx + rot[5] * dy + rot[8] * dz;
  float w   = b->halfWidth;
  float h   = b->halfHeight;
  float d   = b->halfDepth;
  float ox, oy, oz;
  if (ldx < 0.f) {
    ox = -w;
  }
  else {
    ox = w;
  }
  if (ldy < 0.f) {
    oy = -h;
  }
  else {
    oy = h;
  }
  if (ldz < 0.f) {
    oz = -d;
  }
  else {
    oz = d;
  }
  ldx = rot[0] * ox + rot[1] * oy + rot[2] * oz + b->position.x;
  ldy = rot[3] * ox + rot[4] * oy + rot[5] * oz + b->position.y;
  ldz = rot[6] * ox + rot[7] * oy + rot[8] * oz + b->position.z;
  out.init(ldx, ldy, ldz);
}

void BoxCylinderCollisionDetector::supportPointC(CylinderShape* c, float dx,
                                                 float dy, float dz, Vec3& out)
{
  const std::array<float, 9>& rot = c->rotation.elements;
  float ldx  = rot[0] * dx + rot[3] * dy + rot[6] * dz;
  float ldy  = rot[1] * dx + rot[4] * dy + rot[7] * dz;
  float ldz  = rot[2] * dx + rot[5] * dy + rot[8] * dz;
  float radx = ldx;
  float radz = ldz;
  float len  = radx * radx + radz * radz;
  float rad  = c->radius;
  float hh   = c->halfHeight;
  float ox, oy, oz;
  if (floats_are_equal(len, 0.f)) {
    if (ldy < 0.f) {
      ox = rad;
      oy = -hh;
      oz = 0.f;
    }
    else {
      ox = rad;
      oy = hh;
      oz = 0.f;
    }
  }
  else {
    len = c->radius / std::sqrt(len);
    if (ldy < 0.f) {
      ox = radx * len;
      oy = -hh;
      oz = radz * len;
    }
    else {
      ox = radx * len;
      oy = hh;
      oz = radz * len;
    }
  }
  ldx = rot[0] * ox + rot[1] * oy + rot[2] * oz + c->position.x;
  ldy = rot[3] * ox + rot[4] * oy + rot[5] * oz + c->position.y;
  ldz = rot[6] * ox + rot[7] * oy + rot[8] * oz + c->position.z;
  out.init(ldx, ldy, ldz);
}

void BoxCylinderCollisionDetector::detectCollision(Shape* shape1, Shape* shape2,
                                                   ContactManifold* manifold)
{
  Shape *_b, *_c;
  if (flip) {
    _b = shape2;
    _c = shape1;
  }
  else {
    _b = shape1;
    _c = shape2;
  }
  if (_b->type != Shape::Type::SHAPE_BOX
      || _c->type != Shape::Type::SHAPE_CYLINDER) {
    return;
  }
  BoxShape* b      = dynamic_cast<BoxShape*>(_b);
  CylinderShape* c = dynamic_cast<CylinderShape*>(_c);

  Vec3 sep, pos, dep;
  if (!getSep(b, c, sep, pos, dep)) {
    return;
  }
  float pbx = b->position.x;
  float pby = b->position.y;
  float pbz = b->position.z;
  float pcx = c->position.x;
  float pcy = c->position.y;
  float pcz = c->position.z;
  float bw  = b->halfWidth;
  float bh  = b->halfHeight;
  float bd  = b->halfDepth;
  float ch  = c->halfHeight;
  float r   = c->radius;

  const std::array<float, 18>& D = b->dimensions;

  float nwx = D[0]; // b.normalDirectionWidth.x;
  float nwy = D[1]; // b.normalDirectionWidth.y;
  float nwz = D[2]; // b.normalDirectionWidth.z;
  float nhx = D[3]; // b.normalDirectionHeight.x;
  float nhy = D[4]; // b.normalDirectionHeight.y;
  float nhz = D[5]; // b.normalDirectionHeight.z;
  float ndx = D[6]; // b.normalDirectionDepth.x;
  float ndy = D[7]; // b.normalDirectionDepth.y;
  float ndz = D[8]; // b.normalDirectionDepth.z;

  float dwx = D[9];  // b.halfDirectionWidth.x;
  float dwy = D[10]; // b.halfDirectionWidth.y;
  float dwz = D[11]; // b.halfDirectionWidth.z;
  float dhx = D[12]; // b.halfDirectionHeight.x;
  float dhy = D[13]; // b.halfDirectionHeight.y;
  float dhz = D[14]; // b.halfDirectionHeight.z;
  float ddx = D[15]; // b.halfDirectionDepth.x;
  float ddy = D[16]; // b.halfDirectionDepth.y;
  float ddz = D[17]; // b.halfDirectionDepth.z;

  float ncx   = c->normalDirection.x;
  float ncy   = c->normalDirection.y;
  float ncz   = c->normalDirection.z;
  float dcx   = c->halfDirection.x;
  float dcy   = c->halfDirection.y;
  float dcz   = c->halfDirection.z;
  float nx    = sep.x;
  float ny    = sep.y;
  float nz    = sep.z;
  float dotw  = nx * nwx + ny * nwy + nz * nwz;
  float doth  = nx * nhx + ny * nhy + nz * nhz;
  float dotd  = nx * ndx + ny * ndy + nz * ndz;
  float dotc  = nx * ncx + ny * ncy + nz * ncz;
  bool right1 = dotw > 0;
  bool right2 = doth > 0;
  bool right3 = dotd > 0;
  bool right4 = dotc > 0;
  if (!right1) {
    dotw = -dotw;
  }
  if (!right2) {
    doth = -doth;
  }
  if (!right3) {
    dotd = -dotd;
  }
  if (!right4) {
    dotc = -dotc;
  }
  unsigned int state = 0;
  if (dotc > 0.999f) {
    if (dotw > 0.999f) {
      if (dotw > dotc) {
        state = 1;
      }
      else {
        state = 4;
      }
    }
    else if (doth > 0.999f) {
      if (doth > dotc) {
        state = 2;
      }
      else {
        state = 4;
      }
    }
    else if (dotd > 0.999f) {
      if (dotd > dotc) {
        state = 3;
      }
      else {
        state = 4;
      }
    }
    else {
      state = 4;
    }
  }
  else {
    if (dotw > 0.999f) {
      state = 1;
    }
    else if (doth > 0.999f) {
      state = 2;
    }
    else if (dotd > 0.999f) {
      state = 3;
    }
  }
  float cbx = 0.f, cby = 0.f, cbz = 0.f;
  float ccx, ccy, ccz;
  float r00, r01, r02;
  float r10, r11, r12;
  float r20, r21, r22;
  float px, py, pz;
  float pd;
  float dot;
  float len;
  float tx, ty, tz;
  float td;
  float dx, dy, dz;
  float d1x, d1y, d1z;
  float d2x, d2y, d2z;
  float sx, sy, sz;
  float sd;
  float ex, ey, ez;
  float ed;
  float dot1, dot2;
  float t1;
  float dir1x = 0.f, dir1y = 0.f, dir1z = 0.f;
  float dir2x = 0.f, dir2y = 0.f, dir2z = 0.f;
  float dir1l = 0.f, dir2l = 0.f;
  if (state == 0) {
    manifold->addPoint(pos.x, pos.y, pos.z, nx, ny, nz, dep.x, flip);
  }
  else if (state == 4) {
    if (right4) {
      ccx = pcx - dcx;
      ccy = pcy - dcy;
      ccz = pcz - dcz;
      nx  = -ncx;
      ny  = -ncy;
      nz  = -ncz;
    }
    else {
      ccx = pcx + dcx;
      ccy = pcy + dcy;
      ccz = pcz + dcz;
      nx  = ncx;
      ny  = ncy;
      nz  = ncz;
    }
    float v1x = 0.f, v1y = 0.f, v1z = 0.f;
    float v2x = 0.f, v2y = 0.f, v2z = 0.f;
    float v3x = 0.f, v3y = 0.f, v3z = 0.f;
    float v4x = 0.f, v4y = 0.f, v4z = 0.f;

    dot   = 1.f;
    state = 0;
    dot1  = nwx * nx + nwy * ny + nwz * nz;
    if (dot1 < dot) {
      dot   = dot1;
      state = 0;
    }
    if (-dot1 < dot) {
      dot   = -dot1;
      state = 1;
    }
    dot1 = nhx * nx + nhy * ny + nhz * nz;
    if (dot1 < dot) {
      dot   = dot1;
      state = 2;
    }
    if (-dot1 < dot) {
      dot   = -dot1;
      state = 3;
    }
    dot1 = ndx * nx + ndy * ny + ndz * nz;
    if (dot1 < dot) {
      dot   = dot1;
      state = 4;
    }
    if (-dot1 < dot) {
      dot   = -dot1;
      state = 5;
    }
    const std::array<float, 24>& v = b->elements;
    switch (state) {
      case 0:
        // v=b.vertex1;
        v1x = v[0]; // v.x;
        v1y = v[1]; // v.y;
        v1z = v[2]; // v.z;
        // v=b.vertex3;
        v2x = v[6]; // v.x;
        v2y = v[7]; // v.y;
        v2z = v[8]; // v.z;
        // v=b.vertex4;
        v3x = v[9];  // v.x;
        v3y = v[10]; // v.y;
        v3z = v[11]; // v.z;
        // v=b.vertex2;
        v4x = v[3]; // v.x;
        v4y = v[4]; // v.y;
        v4z = v[5]; // v.z;
        break;
      case 1:
        // v=b.vertex6;
        v1x = v[15]; // v.x;
        v1y = v[16]; // v.y;
        v1z = v[17]; // v.z;
        // v=b.vertex8;
        v2x = v[21]; // v.x;
        v2y = v[22]; // v.y;
        v2z = v[23]; // v.z;
        // v=b.vertex7;
        v3x = v[18]; // v.x;
        v3y = v[19]; // v.y;
        v3z = v[20]; // v.z;
        // v=b.vertex5;
        v4x = v[12]; // v.x;
        v4y = v[13]; // v.y;
        v4z = v[14]; // v.z;
        break;
      case 2:
        // v=b.vertex5;
        v1x = v[12]; // v.x;
        v1y = v[13]; // v.y;
        v1z = v[14]; // v.z;
        // v=b.vertex1;
        v2x = v[0]; // v.x;
        v2y = v[1]; // v.y;
        v2z = v[2]; // v.z;
        // v=b.vertex2;
        v3x = v[3]; // v.x;
        v3y = v[4]; // v.y;
        v3z = v[5]; // v.z;
        // v=b.vertex6;
        v4x = v[15]; // v.x;
        v4y = v[16]; // v.y;
        v4z = v[17]; // v.z;
        break;
      case 3:
        // v=b.vertex8;
        v1x = v[21]; // v.x;
        v1y = v[22]; // v.y;
        v1z = v[23]; // v.z;
        // v=b.vertex4;
        v2x = v[9];  // v.x;
        v2y = v[10]; // v.y;
        v2z = v[11]; // v.z;
        // v=b.vertex3;
        v3x = v[6]; // v.x;
        v3y = v[7]; // v.y;
        v3z = v[8]; // v.z;
        // v=b.vertex7;
        v4x = v[18]; // v.x;
        v4y = v[19]; // v.y;
        v4z = v[20]; // v.z;
        break;
      case 4:
        // v=b.vertex5;
        v1x = v[12]; // v.x;
        v1y = v[13]; // v.y;
        v1z = v[14]; // v.z;
        // v=b.vertex7;
        v2x = v[18]; // v.x;
        v2y = v[19]; // v.y;
        v2z = v[20]; // v.z;
        // v=b.vertex3;
        v3x = v[6]; // v.x;
        v3y = v[7]; // v.y;
        v3z = v[8]; // v.z;
        // v=b.vertex1;
        v4x = v[0]; // v.x;
        v4y = v[1]; // v.y;
        v4z = v[2]; // v.z;
        break;
      case 5:
        // v=b.vertex2;
        v1x = v[3]; // v.x;
        v1y = v[4]; // v.y;
        v1z = v[5]; // v.z;
        // v=b.vertex4;
        v2x = v[9];  // v.x;
        v2y = v[10]; // v.y;
        v2z = v[11]; // v.z;
        // v=b.vertex8;
        v3x = v[21]; // v.x;
        v3y = v[22]; // v.y;
        v3z = v[23]; // v.z;
        // v=b.vertex6;
        v4x = v[15]; // v.x;
        v4y = v[16]; // v.y;
        v4z = v[17]; // v.z;
        break;
      default:
        break;
    }
    pd = nx * (v1x - ccx) + ny * (v1y - ccy) + nz * (v1z - ccz);
    if (pd <= 0) {
      manifold->addPoint(v1x, v1y, v1z, -nx, -ny, -nz, pd, flip);
    }
    pd = nx * (v2x - ccx) + ny * (v2y - ccy) + nz * (v2z - ccz);
    if (pd <= 0) {
      manifold->addPoint(v2x, v2y, v2z, -nx, -ny, -nz, pd, flip);
    }
    pd = nx * (v3x - ccx) + ny * (v3y - ccy) + nz * (v3z - ccz);
    if (pd <= 0) {
      manifold->addPoint(v3x, v3y, v3z, -nx, -ny, -nz, pd, flip);
    }
    pd = nx * (v4x - ccx) + ny * (v4y - ccy) + nz * (v4z - ccz);
    if (pd <= 0) {
      manifold->addPoint(v4x, v4y, v4z, -nx, -ny, -nz, pd, flip);
    }
  }
  else {
    switch (state) {
      case 1:
        if (right1) {
          cbx = pbx + dwx;
          cby = pby + dwy;
          cbz = pbz + dwz;
          nx  = nwx;
          ny  = nwy;
          nz  = nwz;
        }
        else {
          cbx = pbx - dwx;
          cby = pby - dwy;
          cbz = pbz - dwz;
          nx  = -nwx;
          ny  = -nwy;
          nz  = -nwz;
        }
        dir1x = nhx;
        dir1y = nhy;
        dir1z = nhz;
        dir1l = bh;
        dir2x = ndx;
        dir2y = ndy;
        dir2z = ndz;
        dir2l = bd;
        break;
      case 2:
        if (right2) {
          cbx = pbx + dhx;
          cby = pby + dhy;
          cbz = pbz + dhz;
          nx  = nhx;
          ny  = nhy;
          nz  = nhz;
        }
        else {
          cbx = pbx - dhx;
          cby = pby - dhy;
          cbz = pbz - dhz;
          nx  = -nhx;
          ny  = -nhy;
          nz  = -nhz;
        }
        dir1x = nwx;
        dir1y = nwy;
        dir1z = nwz;
        dir1l = bw;
        dir2x = ndx;
        dir2y = ndy;
        dir2z = ndz;
        dir2l = bd;
        break;
      case 3:
        if (right3) {
          cbx = pbx + ddx;
          cby = pby + ddy;
          cbz = pbz + ddz;
          nx  = ndx;
          ny  = ndy;
          nz  = ndz;
        }
        else {
          cbx = pbx - ddx;
          cby = pby - ddy;
          cbz = pbz - ddz;
          nx  = -ndx;
          ny  = -ndy;
          nz  = -ndz;
        }
        dir1x = nwx;
        dir1y = nwy;
        dir1z = nwz;
        dir1l = bw;
        dir2x = nhx;
        dir2y = nhy;
        dir2z = nhz;
        dir2l = bh;
        break;
      default:
        break;
    }
    dot = nx * ncx + ny * ncy + nz * ncz;
    if (dot < 0.f) {
      len = ch;
    }
    else {
      len = -ch;
    }
    ccx = pcx + len * ncx;
    ccy = pcy + len * ncy;
    ccz = pcz + len * ncz;
    if (dotc >= 0.999999f) {
      tx = -ny;
      ty = nz;
      tz = nx;
    }
    else {
      tx = nx;
      ty = ny;
      tz = nz;
    }
    len = tx * ncx + ty * ncy + tz * ncz;
    dx  = len * ncx - tx;
    dy  = len * ncy - ty;
    dz  = len * ncz - tz;
    len = std::sqrt(dx * dx + dy * dy + dz * dz);
    if (floats_are_equal(len, 0)) {
      return;
    }
    len = r / len;
    dx *= len;
    dy *= len;
    dz *= len;
    tx = ccx + dx;
    ty = ccy + dy;
    tz = ccz + dz;
    if (dot < -0.96f || dot > 0.96f) {
      r00 = ncx * ncx * 1.5f - 0.5f;
      r01 = ncx * ncy * 1.5f - ncz * 0.866025403f;
      r02 = ncx * ncz * 1.5f + ncy * 0.866025403f;
      r10 = ncy * ncx * 1.5f + ncz * 0.866025403f;
      r11 = ncy * ncy * 1.5f - 0.5f;
      r12 = ncy * ncz * 1.5f - ncx * 0.866025403f;
      r20 = ncz * ncx * 1.5f - ncy * 0.866025403f;
      r21 = ncz * ncy * 1.5f + ncx * 0.866025403f;
      r22 = ncz * ncz * 1.5f - 0.5f;
      px  = tx;
      py  = ty;
      pz  = tz;
      pd  = nx * (px - cbx) + ny * (py - cby) + nz * (pz - cbz);
      tx  = px - pd * nx - cbx;
      ty  = py - pd * ny - cby;
      tz  = pz - pd * nz - cbz;
      sd  = dir1x * tx + dir1y * ty + dir1z * tz;
      ed  = dir2x * tx + dir2y * ty + dir2z * tz;
      if (sd < -dir1l) {
        sd = -dir1l;
      }
      else if (sd > dir1l) {
        sd = dir1l;
      }
      if (ed < -dir2l) {
        ed = -dir2l;
      }
      else if (ed > dir2l) {
        ed = dir2l;
      }
      tx = sd * dir1x + ed * dir2x;
      ty = sd * dir1y + ed * dir2y;
      tz = sd * dir1z + ed * dir2z;
      px = cbx + tx;
      py = cby + ty;
      pz = cbz + tz;
      manifold->addPoint(px, py, pz, nx, ny, nz, pd, flip);
      px = dx * r00 + dy * r01 + dz * r02;
      py = dx * r10 + dy * r11 + dz * r12;
      pz = dx * r20 + dy * r21 + dz * r22;
      px = (dx = px) + ccx;
      py = (dy = py) + ccy;
      pz = (dz = pz) + ccz;
      pd = nx * (px - cbx) + ny * (py - cby) + nz * (pz - cbz);
      if (pd <= 0) {
        tx = px - pd * nx - cbx;
        ty = py - pd * ny - cby;
        tz = pz - pd * nz - cbz;
        sd = dir1x * tx + dir1y * ty + dir1z * tz;
        ed = dir2x * tx + dir2y * ty + dir2z * tz;
        if (sd < -dir1l) {
          sd = -dir1l;
        }
        else if (sd > dir1l) {
          sd = dir1l;
        }
        if (ed < -dir2l) {
          ed = -dir2l;
        }
        else if (ed > dir2l) {
          ed = dir2l;
        }
        tx = sd * dir1x + ed * dir2x;
        ty = sd * dir1y + ed * dir2y;
        tz = sd * dir1z + ed * dir2z;
        px = cbx + tx;
        py = cby + ty;
        pz = cbz + tz;
        manifold->addPoint(px, py, pz, nx, ny, nz, pd, flip);
      }
      px = dx * r00 + dy * r01 + dz * r02;
      py = dx * r10 + dy * r11 + dz * r12;
      pz = dx * r20 + dy * r21 + dz * r22;
      px = (dx = px) + ccx;
      py = (dy = py) + ccy;
      pz = (dz = pz) + ccz;
      pd = nx * (px - cbx) + ny * (py - cby) + nz * (pz - cbz);
      if (pd <= 0.f) {
        tx = px - pd * nx - cbx;
        ty = py - pd * ny - cby;
        tz = pz - pd * nz - cbz;
        sd = dir1x * tx + dir1y * ty + dir1z * tz;
        ed = dir2x * tx + dir2y * ty + dir2z * tz;
        if (sd < -dir1l) {
          sd = -dir1l;
        }
        else if (sd > dir1l) {
          sd = dir1l;
        }
        if (ed < -dir2l) {
          ed = -dir2l;
        }
        else if (ed > dir2l) {
          ed = dir2l;
        }
        tx = sd * dir1x + ed * dir2x;
        ty = sd * dir1y + ed * dir2y;
        tz = sd * dir1z + ed * dir2z;
        px = cbx + tx;
        py = cby + ty;
        pz = cbz + tz;
        manifold->addPoint(px, py, pz, nx, ny, nz, pd, flip);
      }
    }
    else {
      sx = tx;
      sy = ty;
      sz = tz;
      sd = nx * (sx - cbx) + ny * (sy - cby) + nz * (sz - cbz);
      sx -= sd * nx;
      sy -= sd * ny;
      sz -= sd * nz;
      if (dot > 0.f) {
        ex = tx + dcx * 2.f;
        ey = ty + dcy * 2.f;
        ez = tz + dcz * 2.f;
      }
      else {
        ex = tx - dcx * 2.f;
        ey = ty - dcy * 2.f;
        ez = tz - dcz * 2.f;
      }
      ed = nx * (ex - cbx) + ny * (ey - cby) + nz * (ez - cbz);
      ex -= ed * nx;
      ey -= ed * ny;
      ez -= ed * nz;
      d1x  = sx - cbx;
      d1y  = sy - cby;
      d1z  = sz - cbz;
      d2x  = ex - cbx;
      d2y  = ey - cby;
      d2z  = ez - cbz;
      tx   = ex - sx;
      ty   = ey - sy;
      tz   = ez - sz;
      td   = ed - sd;
      dotw = d1x * dir1x + d1y * dir1y + d1z * dir1z;
      doth = d2x * dir1x + d2y * dir1y + d2z * dir1z;
      dot1 = dotw - dir1l;
      dot2 = doth - dir1l;
      if (dot1 > 0.f) {
        if (dot2 > 0.f) {
          return;
        }
        t1   = dot1 / (dot1 - dot2);
        sx   = sx + tx * t1;
        sy   = sy + ty * t1;
        sz   = sz + tz * t1;
        sd   = sd + td * t1;
        d1x  = sx - cbx;
        d1y  = sy - cby;
        d1z  = sz - cbz;
        dotw = d1x * dir1x + d1y * dir1y + d1z * dir1z;
        tx   = ex - sx;
        ty   = ey - sy;
        tz   = ez - sz;
        td   = ed - sd;
      }
      else if (dot2 > 0.f) {
        t1   = dot1 / (dot1 - dot2);
        ex   = sx + tx * t1;
        ey   = sy + ty * t1;
        ez   = sz + tz * t1;
        ed   = sd + td * t1;
        d2x  = ex - cbx;
        d2y  = ey - cby;
        d2z  = ez - cbz;
        doth = d2x * dir1x + d2y * dir1y + d2z * dir1z;
        tx   = ex - sx;
        ty   = ey - sy;
        tz   = ez - sz;
        td   = ed - sd;
      }
      dot1 = dotw + dir1l;
      dot2 = doth + dir1l;
      if (dot1 < 0.f) {
        if (dot2 < 0.f) {
          return;
        }
        t1  = dot1 / (dot1 - dot2);
        sx  = sx + tx * t1;
        sy  = sy + ty * t1;
        sz  = sz + tz * t1;
        sd  = sd + td * t1;
        d1x = sx - cbx;
        d1y = sy - cby;
        d1z = sz - cbz;
        tx  = ex - sx;
        ty  = ey - sy;
        tz  = ez - sz;
        td  = ed - sd;
      }
      else if (dot2 < 0.f) {
        t1  = dot1 / (dot1 - dot2);
        ex  = sx + tx * t1;
        ey  = sy + ty * t1;
        ez  = sz + tz * t1;
        ed  = sd + td * t1;
        d2x = ex - cbx;
        d2y = ey - cby;
        d2z = ez - cbz;
        tx  = ex - sx;
        ty  = ey - sy;
        tz  = ez - sz;
        td  = ed - sd;
      }
      dotw = d1x * dir2x + d1y * dir2y + d1z * dir2z;
      doth = d2x * dir2x + d2y * dir2y + d2z * dir2z;
      dot1 = dotw - dir2l;
      dot2 = doth - dir2l;
      if (dot1 > 0.f) {
        if (dot2 > 0) {
          return;
        }
        t1   = dot1 / (dot1 - dot2);
        sx   = sx + tx * t1;
        sy   = sy + ty * t1;
        sz   = sz + tz * t1;
        sd   = sd + td * t1;
        d1x  = sx - cbx;
        d1y  = sy - cby;
        d1z  = sz - cbz;
        dotw = d1x * dir2x + d1y * dir2y + d1z * dir2z;
        tx   = ex - sx;
        ty   = ey - sy;
        tz   = ez - sz;
        td   = ed - sd;
      }
      else if (dot2 > 0.f) {
        t1   = dot1 / (dot1 - dot2);
        ex   = sx + tx * t1;
        ey   = sy + ty * t1;
        ez   = sz + tz * t1;
        ed   = sd + td * t1;
        d2x  = ex - cbx;
        d2y  = ey - cby;
        d2z  = ez - cbz;
        doth = d2x * dir2x + d2y * dir2y + d2z * dir2z;
        tx   = ex - sx;
        ty   = ey - sy;
        tz   = ez - sz;
        td   = ed - sd;
      }
      dot1 = dotw + dir2l;
      dot2 = doth + dir2l;
      if (dot1 < 0.f) {
        if (dot2 < 0.f) {
          return;
        }
        t1 = dot1 / (dot1 - dot2);
        sx = sx + tx * t1;
        sy = sy + ty * t1;
        sz = sz + tz * t1;
        sd = sd + td * t1;
      }
      else if (dot2 < 0.f) {
        t1 = dot1 / (dot1 - dot2);
        ex = sx + tx * t1;
        ey = sy + ty * t1;
        ez = sz + tz * t1;
        ed = sd + td * t1;
      }
      if (sd < 0.f) {
        manifold->addPoint(sx, sy, sz, nx, ny, nz, sd, flip);
      }
      if (ed < 0.f) {
        manifold->addPoint(ex, ey, ez, nx, ny, nz, ed, flip);
      }
    }
  }
}

} // end of namespace OIMO
