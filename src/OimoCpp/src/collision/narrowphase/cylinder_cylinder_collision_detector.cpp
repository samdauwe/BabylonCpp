#include <oimo/collision/narrowphase/cylinder_cylinder_collision_detector.h>

#include <cmath>

#include <oimo/collision/shape/cylinder_shape.h>
#include <oimo/constraint/contact/contact_manifold.h>
#include <oimo/math/vec3.h>
#include <oimo/oimo_utils.h>

namespace OIMO {

CylinderCylinderCollisionDetector::CylinderCylinderCollisionDetector()
    : CollisionDetector{}
{
}

CylinderCylinderCollisionDetector::~CylinderCylinderCollisionDetector()
{
}

bool CylinderCylinderCollisionDetector::getSep(CylinderShape* c1,
                                               CylinderShape* c2, Vec3& sep,
                                               Vec3& pos, Vec3& dep)
{
  float t1x, t1y, t1z;
  float t2x, t2y, t2z;
  Vec3 sup;
  float len;
  float p1x = 0.f, p1y = 0.f, p1z = 0.f;
  float p2x = 0.f, p2y = 0.f, p2z = 0.f;
  float v01x = c1->position.x;
  float v01y = c1->position.y;
  float v01z = c1->position.z;
  float v02x = c2->position.x;
  float v02y = c2->position.y;
  float v02z = c2->position.z;
  float v0x  = v02x - v01x;
  float v0y  = v02y - v01y;
  float v0z  = v02z - v01z;
  if (floats_are_equal(v0x * v0x + v0y * v0y + v0z * v0z, 0.f)) {
    v0y = 0.001f;
  }
  float nx = -v0x;
  float ny = -v0y;
  float nz = -v0z;
  supportPoint(c1, -nx, -ny, -nz, sup);
  float v11x = sup.x;
  float v11y = sup.y;
  float v11z = sup.z;
  supportPoint(c2, nx, ny, nz, sup);
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
    sep.set(v1x - v0x, v1y - v0y, v1z - v0z).normalize();
    pos.set((v11x + v12x) * 0.5f, (v11y + v12y) * 0.5f, (v11z + v12z) * 0.5f);
    return true;
  }
  supportPoint(c1, -nx, -ny, -nz, sup);
  float v21x = sup.x;
  float v21y = sup.y;
  float v21z = sup.z;
  supportPoint(c2, nx, ny, nz, sup);
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
    supportPoint(c1, -nx, -ny, -nz, sup);
    v31x = sup.x;
    v31y = sup.y;
    v31z = sup.z;
    supportPoint(c2, nx, ny, nz, sup);
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
        if (sum <= 0) {
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
      supportPoint(c1, -nx, -ny, -nz, sup);
      v41x = sup.x;
      v41y = sup.y;
      v41z = sup.z;
      supportPoint(c2, nx, ny, nz, sup);
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
          sep.set(-nx, -ny, -nz);
          pos.set((p1x + p2x) * 0.5f, (p1y + p2y) * 0.5f, (p1z + p2z) * 0.5f);
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
}

void CylinderCylinderCollisionDetector::supportPoint(CylinderShape* c, float dx,
                                                     float dy, float dz,
                                                     Vec3& out)
{
  const auto& rot = c->rotation.elements;
  float ldx       = rot[0] * dx + rot[3] * dy + rot[6] * dz;
  float ldy       = rot[1] * dx + rot[4] * dy + rot[7] * dz;
  float ldz       = rot[2] * dx + rot[5] * dy + rot[8] * dz;
  float radx      = ldx;
  float radz      = ldz;
  float len       = radx * radx + radz * radz;
  float rad       = c->radius;
  float hh        = c->halfHeight;
  float ox, oy, oz;
  if (floats_are_equal(len, 0)) {
    if (ldy < 0) {
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
  out.set(ldx, ldy, ldz);
}

void CylinderCylinderCollisionDetector::detectCollision(
  Shape* shape1, Shape* shape2, ContactManifold* manifold)
{
  if (shape1->type != Shape::Type::SHAPE_CYLINDER
      || shape2->type != Shape::Type::SHAPE_CYLINDER) {
    return;
  }
  Shape *_c1, *_c2;
  if (shape1->id < shape2->id) {
    _c1 = shape1;
    _c2 = shape2;
  }
  else {
    _c1 = shape2;
    _c2 = shape1;
  }
  auto c1 = dynamic_cast<CylinderShape*>(_c1);
  auto c2 = dynamic_cast<CylinderShape*>(_c2);

  const auto& p1 = c1->position;
  const auto& p2 = c2->position;

  float p1x = p1.x;
  float p1y = p1.y;
  float p1z = p1.z;
  float p2x = p2.x;
  float p2y = p2.y;
  float p2z = p2.z;
  float h1  = c1->halfHeight;
  float h2  = c2->halfHeight;

  const auto& n1 = c1->normalDirection;
  const auto& n2 = c2->normalDirection;
  const auto& d1 = c1->halfDirection;
  const auto& d2 = c2->halfDirection;

  float r1  = c1->radius;
  float r2  = c2->radius;
  float n1x = n1.x;
  float n1y = n1.y;
  float n1z = n1.z;
  float n2x = n2.x;
  float n2y = n2.y;
  float n2z = n2.z;
  float d1x = d1.x;
  float d1y = d1.y;
  float d1z = d1.z;
  float d2x = d2.x;
  float d2y = d2.y;
  float d2z = d2.z;
  float dx  = p1x - p2x;
  float dy  = p1y - p2y;
  float dz  = p1z - p2z;
  float len;
  float c1x, c1y, c1z;
  float c2x, c2y, c2z;
  float tx, ty, tz;
  float sx, sy, sz;
  float ex, ey, ez;
  float depth1, depth2;
  float dot;
  float t1, t2;
  Vec3 sep, pos, dep;
  if (!getSep(c1, c2, sep, pos, dep)) {
    return;
  }
  float dot1  = sep.x * n1x + sep.y * n1y + sep.z * n1z;
  float dot2  = sep.x * n2x + sep.y * n2y + sep.z * n2z;
  bool right1 = dot1 > 0.f;
  bool right2 = dot2 > 0.f;
  if (!right1) {
    dot1 = -dot1;
  }
  if (!right2) {
    dot2 = -dot2;
  }
  unsigned int state = 0;
  if (dot1 > 0.999f || dot2 > 0.999f) {
    if (dot1 > dot2) {
      state = 1;
    }
    else {
      state = 2;
    }
  }
  float nx, ny, nz;
  float depth = dep.x;
  float r00, r01, r02;
  float r10, r11, r12;
  float r20, r21, r22;
  float px = 0.f, py = 0.f, pz = 0.f, pd = 0.f;
  float a, b, e, f;
  nx = sep.x;
  ny = sep.y;
  nz = sep.z;
  switch (state) {
    case 0:
      manifold->addPoint(pos.x, pos.y, pos.z, nx, ny, nz, depth, false);
      break;
    case 1:
      if (right1) {
        c1x = p1x + d1x;
        c1y = p1y + d1y;
        c1z = p1z + d1z;
        nx  = n1x;
        ny  = n1y;
        nz  = n1z;
      }
      else {
        c1x = p1x - d1x;
        c1y = p1y - d1y;
        c1z = p1z - d1z;
        nx  = -n1x;
        ny  = -n1y;
        nz  = -n1z;
      }
      dot = nx * n2x + ny * n2y + nz * n2z;
      if (dot < 0.f) {
        len = h2;
      }
      else {
        len = -h2;
      }
      c2x = p2x + len * n2x;
      c2y = p2y + len * n2y;
      c2z = p2z + len * n2z;
      if (dot2 >= 0.999999f) {
        tx = -ny;
        ty = nz;
        tz = nx;
      }
      else {
        tx = nx;
        ty = ny;
        tz = nz;
      }
      len = tx * n2x + ty * n2y + tz * n2z;
      dx  = len * n2x - tx;
      dy  = len * n2y - ty;
      dz  = len * n2z - tz;
      len = std::sqrt(dx * dx + dy * dy + dz * dz);
      if (floats_are_equal(len, 0.f)) {
        break;
      }
      len = r2 / len;
      dx *= len;
      dy *= len;
      dz *= len;
      tx = c2x + dx;
      ty = c2y + dy;
      tz = c2z + dz;
      if (dot < -0.96f || dot > 0.96f) {
        r00 = n2x * n2x * 1.5f - 0.5f;
        r01 = n2x * n2y * 1.5f - n2z * 0.866025403f;
        r02 = n2x * n2z * 1.5f + n2y * 0.866025403f;
        r10 = n2y * n2x * 1.5f + n2z * 0.866025403f;
        r11 = n2y * n2y * 1.5f - 0.5f;
        r12 = n2y * n2z * 1.5f - n2x * 0.866025403f;
        r20 = n2z * n2x * 1.5f - n2y * 0.866025403f;
        r21 = n2z * n2y * 1.5f + n2x * 0.866025403f;
        r22 = n2z * n2z * 1.5f - 0.5f;
        px  = tx;
        py  = ty;
        pz  = tz;
        pd  = nx * (px - c1x) + ny * (py - c1y) + nz * (pz - c1z);
        tx  = px - pd * nx - c1x;
        ty  = py - pd * ny - c1y;
        tz  = pz - pd * nz - c1z;
        len = tx * tx + ty * ty + tz * tz;
        if (len > r1 * r1) {
          len = r1 / std::sqrt(len);
          tx *= len;
          ty *= len;
          tz *= len;
        }
        px = c1x + tx;
        py = c1y + ty;
        pz = c1z + tz;
        manifold->addPoint(px, py, pz, nx, ny, nz, pd, false);
        px = dx * r00 + dy * r01 + dz * r02;
        py = dx * r10 + dy * r11 + dz * r12;
        pz = dx * r20 + dy * r21 + dz * r22;
        px = (dx = px) + c2x;
        py = (dy = py) + c2y;
        pz = (dz = pz) + c2z;
        pd = nx * (px - c1x) + ny * (py - c1y) + nz * (pz - c1z);
        if (pd <= 0.f) {
          tx  = px - pd * nx - c1x;
          ty  = py - pd * ny - c1y;
          tz  = pz - pd * nz - c1z;
          len = tx * tx + ty * ty + tz * tz;
          if (len > r1 * r1) {
            len = r1 / std::sqrt(len);
            tx *= len;
            ty *= len;
            tz *= len;
          }
          px = c1x + tx;
          py = c1y + ty;
          pz = c1z + tz;
          manifold->addPoint(px, py, pz, nx, ny, nz, pd, false);
        }
        px = dx * r00 + dy * r01 + dz * r02;
        py = dx * r10 + dy * r11 + dz * r12;
        pz = dx * r20 + dy * r21 + dz * r22;
        px = (dx = px) + c2x;
        py = (dy = py) + c2y;
        pz = (dz = pz) + c2z;
        pd = nx * (px - c1x) + ny * (py - c1y) + nz * (pz - c1z);
        if (pd <= 0) {
          tx  = px - pd * nx - c1x;
          ty  = py - pd * ny - c1y;
          tz  = pz - pd * nz - c1z;
          len = tx * tx + ty * ty + tz * tz;
          if (len > r1 * r1) {
            len = r1 / std::sqrt(len);
            tx *= len;
            ty *= len;
            tz *= len;
          }
          px = c1x + tx;
          py = c1y + ty;
          pz = c1z + tz;
          manifold->addPoint(px, py, pz, nx, ny, nz, pd, false);
        }
      }
      else {
        sx     = tx;
        sy     = ty;
        sz     = tz;
        depth1 = nx * (sx - c1x) + ny * (sy - c1y) + nz * (sz - c1z);
        sx -= depth1 * nx;
        sy -= depth1 * ny;
        sz -= depth1 * nz;
        if (dot > 0.f) {
          ex = tx + n2x * h2 * 2.f;
          ey = ty + n2y * h2 * 2.f;
          ez = tz + n2z * h2 * 2.f;
        }
        else {
          ex = tx - n2x * h2 * 2.f;
          ey = ty - n2y * h2 * 2.f;
          ez = tz - n2z * h2 * 2.f;
        }
        depth2 = nx * (ex - c1x) + ny * (ey - c1y) + nz * (ez - c1z);
        ex -= depth2 * nx;
        ey -= depth2 * ny;
        ez -= depth2 * nz;
        dx = c1x - sx;
        dy = c1y - sy;
        dz = c1z - sz;
        tx = ex - sx;
        ty = ey - sy;
        tz = ez - sz;
        a  = dx * dx + dy * dy + dz * dz;
        b  = dx * tx + dy * ty + dz * tz;
        e  = tx * tx + ty * ty + tz * tz;
        f  = b * b - e * (a - r1 * r1);
        if (f < 0.f) {
          break;
        }
        f  = std::sqrt(f);
        t1 = (b + f) / e;
        t2 = (b - f) / e;
        if (t2 < t1) {
          len = t1;
          t1  = t2;
          t2  = len;
        }
        if (t2 > 1.f) {
          t2 = 1.f;
        }
        if (t1 < 0) {
          t1 = 0.f;
        }
        tx     = sx + (ex - sx) * t1;
        ty     = sy + (ey - sy) * t1;
        tz     = sz + (ez - sz) * t1;
        ex     = sx + (ex - sx) * t2;
        ey     = sy + (ey - sy) * t2;
        ez     = sz + (ez - sz) * t2;
        sx     = tx;
        sy     = ty;
        sz     = tz;
        len    = depth1 + (depth2 - depth1) * t1;
        depth2 = depth1 + (depth2 - depth1) * t2;
        depth1 = len;
        if (depth1 < 0.f) {
          manifold->addPoint(sx, sy, sz, nx, ny, nz, pd, false);
        }
        if (depth2 < 0.f) {
          manifold->addPoint(ex, ey, ez, nx, ny, nz, pd, false);
        }
      }
      break;
    case 2:
      if (right2) {
        c2x = p2x - d2x;
        c2y = p2y - d2y;
        c2z = p2z - d2z;
        nx  = -n2x;
        ny  = -n2y;
        nz  = -n2z;
      }
      else {
        c2x = p2x + d2x;
        c2y = p2y + d2y;
        c2z = p2z + d2z;
        nx  = n2x;
        ny  = n2y;
        nz  = n2z;
      }
      dot = nx * n1x + ny * n1y + nz * n1z;
      if (dot < 0.f) {
        len = h1;
      }
      else {
        len = -h1;
      }
      c1x = p1x + len * n1x;
      c1y = p1y + len * n1y;
      c1z = p1z + len * n1z;
      if (dot1 >= 0.999999f) {
        tx = -ny;
        ty = nz;
        tz = nx;
      }
      else {
        tx = nx;
        ty = ny;
        tz = nz;
      }
      len = tx * n1x + ty * n1y + tz * n1z;
      dx  = len * n1x - tx;
      dy  = len * n1y - ty;
      dz  = len * n1z - tz;
      len = std::sqrt(dx * dx + dy * dy + dz * dz);
      if (floats_are_equal(len, 0)) {
        break;
      }
      len = r1 / len;
      dx *= len;
      dy *= len;
      dz *= len;
      tx = c1x + dx;
      ty = c1y + dy;
      tz = c1z + dz;
      if (dot < -0.96f || dot > 0.96f) {
        r00 = n1x * n1x * 1.5f - 0.5f;
        r01 = n1x * n1y * 1.5f - n1z * 0.866025403f;
        r02 = n1x * n1z * 1.5f + n1y * 0.866025403f;
        r10 = n1y * n1x * 1.5f + n1z * 0.866025403f;
        r11 = n1y * n1y * 1.5f - 0.5f;
        r12 = n1y * n1z * 1.5f - n1x * 0.866025403f;
        r20 = n1z * n1x * 1.5f - n1y * 0.866025403f;
        r21 = n1z * n1y * 1.5f + n1x * 0.866025403f;
        r22 = n1z * n1z * 1.5f - 0.5f;
        px  = tx;
        py  = ty;
        pz  = tz;
        pd  = nx * (px - c2x) + ny * (py - c2y) + nz * (pz - c2z);
        tx  = px - pd * nx - c2x;
        ty  = py - pd * ny - c2y;
        tz  = pz - pd * nz - c2z;
        len = tx * tx + ty * ty + tz * tz;
        if (len > r2 * r2) {
          len = r2 / std::sqrt(len);
          tx *= len;
          ty *= len;
          tz *= len;
        }
        px = c2x + tx;
        py = c2y + ty;
        pz = c2z + tz;
        manifold->addPoint(px, py, pz, -nx, -ny, -nz, pd, false);
        px = dx * r00 + dy * r01 + dz * r02;
        py = dx * r10 + dy * r11 + dz * r12;
        pz = dx * r20 + dy * r21 + dz * r22;
        px = (dx = px) + c1x;
        py = (dy = py) + c1y;
        pz = (dz = pz) + c1z;
        pd = nx * (px - c2x) + ny * (py - c2y) + nz * (pz - c2z);
        if (pd <= 0.f) {
          tx  = px - pd * nx - c2x;
          ty  = py - pd * ny - c2y;
          tz  = pz - pd * nz - c2z;
          len = tx * tx + ty * ty + tz * tz;
          if (len > r2 * r2) {
            len = r2 / std::sqrt(len);
            tx *= len;
            ty *= len;
            tz *= len;
          }
          px = c2x + tx;
          py = c2y + ty;
          pz = c2z + tz;
          manifold->addPoint(px, py, pz, -nx, -ny, -nz, pd, false);
        }
        px = dx * r00 + dy * r01 + dz * r02;
        py = dx * r10 + dy * r11 + dz * r12;
        pz = dx * r20 + dy * r21 + dz * r22;
        px = (dx = px) + c1x;
        py = (dy = py) + c1y;
        pz = (dz = pz) + c1z;
        pd = nx * (px - c2x) + ny * (py - c2y) + nz * (pz - c2z);
        if (pd <= 0.f) {
          tx  = px - pd * nx - c2x;
          ty  = py - pd * ny - c2y;
          tz  = pz - pd * nz - c2z;
          len = tx * tx + ty * ty + tz * tz;
          if (len > r2 * r2) {
            len = r2 / std::sqrt(len);
            tx *= len;
            ty *= len;
            tz *= len;
          }
          px = c2x + tx;
          py = c2y + ty;
          pz = c2z + tz;
          manifold->addPoint(px, py, pz, -nx, -ny, -nz, pd, false);
        }
      }
      else {
        sx     = tx;
        sy     = ty;
        sz     = tz;
        depth1 = nx * (sx - c2x) + ny * (sy - c2y) + nz * (sz - c2z);
        sx -= depth1 * nx;
        sy -= depth1 * ny;
        sz -= depth1 * nz;
        if (dot > 0.f) {
          ex = tx + n1x * h1 * 2.f;
          ey = ty + n1y * h1 * 2.f;
          ez = tz + n1z * h1 * 2.f;
        }
        else {
          ex = tx - n1x * h1 * 2.f;
          ey = ty - n1y * h1 * 2.f;
          ez = tz - n1z * h1 * 2.f;
        }
        depth2 = nx * (ex - c2x) + ny * (ey - c2y) + nz * (ez - c2z);
        ex -= depth2 * nx;
        ey -= depth2 * ny;
        ez -= depth2 * nz;
        dx = c2x - sx;
        dy = c2y - sy;
        dz = c2z - sz;
        tx = ex - sx;
        ty = ey - sy;
        tz = ez - sz;
        a  = dx * dx + dy * dy + dz * dz;
        b  = dx * tx + dy * ty + dz * tz;
        e  = tx * tx + ty * ty + tz * tz;
        f  = b * b - e * (a - r2 * r2);
        if (f < 0.f) {
          break;
        }
        f  = std::sqrt(f);
        t1 = (b + f) / e;
        t2 = (b - f) / e;
        if (t2 < t1) {
          len = t1;
          t1  = t2;
          t2  = len;
        }
        if (t2 > 1.f) {
          t2 = 1.f;
        }
        if (t1 < 0.f) {
          t1 = 0.f;
        }
        tx     = sx + (ex - sx) * t1;
        ty     = sy + (ey - sy) * t1;
        tz     = sz + (ez - sz) * t1;
        ex     = sx + (ex - sx) * t2;
        ey     = sy + (ey - sy) * t2;
        ez     = sz + (ez - sz) * t2;
        sx     = tx;
        sy     = ty;
        sz     = tz;
        len    = depth1 + (depth2 - depth1) * t1;
        depth2 = depth1 + (depth2 - depth1) * t2;
        depth1 = len;
        if (depth1 < 0.f) {
          manifold->addPoint(sx, sy, sz, -nx, -ny, -nz, depth1, false);
        }
        if (depth2 < 0.f) {
          manifold->addPoint(ex, ey, ez, -nx, -ny, -nz, depth2, false);
        }
      }
      break;
    default:
      break;
  }
}

} // end of namespace OIMO
