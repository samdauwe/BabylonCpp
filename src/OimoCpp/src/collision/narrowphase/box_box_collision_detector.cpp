#include <oimo/collision/narrowphase/box_box_collision_detector.h>

#include <cmath>
#include <limits>

#include <oimo/collision/shape/box_shape.h>
#include <oimo/constraint/contact/contact_manifold.h>
#include <oimo/math/vec3.h>
#include <oimo/oimo_utils.h>

namespace OIMO {

BoxBoxCollisionDetector::BoxBoxCollisionDetector()
    : CollisionDetector{}, _inf{std::numeric_limits<float>::infinity()}
{
}

BoxBoxCollisionDetector::~BoxBoxCollisionDetector()
{
}

void BoxBoxCollisionDetector::detectCollision(Shape* shape1, Shape* shape2,
                                              ContactManifold* manifold)
{
  // Approach:
  // * Prepare a separate axis of the fifteen, Six in each of three normal
  //   vectors of the xyz direction of the box both
  // * Remaining nine 3x3 a vector perpendicular to the side of the box 2 and
  //   the side of the box 1
  // * Calculate the depth to the separation axis
  //
  // Calculates the distance using the inner product and put the amount of
  // embedment
  // * However a vertical separation axis and side to weight a little to avoid
  //   vibration and end when there is a separate axis that is remote even one
  // * Look for separation axis with little to dent most
  //   Men and if separation axis of the first six - end collision
  //   Heng If it separate axis of nine other - side collision
  //   Heng - case of a side collision
  // * Find points of two sides on which you made ​​the separation axis
  //
  // Calculates the point of closest approach of a straight line consisting of
  // separate axis points obtained, and the collision point
  // - Surface - the case of the plane crash
  // - Box A, box B and the other a box of better made ​​a separate axis
  // * The surface A and the plane that made the separation axis of the box A,
  //   and B to the surface the face of the box B close in the opposite
  //   direction to the most isolated axis
  //
  // When viewed from the front surface A, and the cut part exceeding the area
  // of the surface A is a surface B
  // -Plane B becomes the 3-8 triangle, I a candidate for the collision point
  // the
  //  vertex of surface B
  // * If more than one candidate 5 exists, scraping up to four
  //
  // For potential collision points of all, to examine the distance between the
  // surface A
  // * If you were on the inside surface of A, and the collision point

  if (shape1->type != Shape::Type::SHAPE_BOX
      || shape2->type != Shape::Type::SHAPE_BOX) {
    return;
  }
  Shape *_b1, *_b2;
  if (shape1->id < shape2->id) {
    _b1 = shape1;
    _b2 = shape2;
  }
  else {
    _b1 = shape2;
    _b2 = shape1;
  }
  auto b1 = dynamic_cast<BoxShape*>(_b1);
  auto b2 = dynamic_cast<BoxShape*>(_b2);

  const auto& V1 = b1->elements;
  const auto& V2 = b2->elements;

  const auto& D1 = b1->dimensions;
  const auto& D2 = b2->dimensions;

  auto p1   = b1->position;
  auto p2   = b2->position;
  float p1x = p1.x;
  float p1y = p1.y;
  float p1z = p1.z;
  float p2x = p2.x;
  float p2y = p2.y;
  float p2z = p2.z;
  // diff
  float dx = p2x - p1x;
  float dy = p2y - p1y;
  float dz = p2z - p1z;
  // distance
  float w1 = b1->halfWidth;
  float h1 = b1->halfHeight;
  float d1 = b1->halfDepth;
  float w2 = b2->halfWidth;
  float h2 = b2->halfHeight;
  float d2 = b2->halfDepth;
  // direction

  // ---------------------------------------------------------------------------
  // 15 separating axes
  // 1~6: face
  // 7~f: edge
  // http://marupeke296.com/COL_3D_No13_OBBvsOBB.html
  // ---------------------------------------------------------------------------

  float a1x = D1[0];
  float a1y = D1[1];
  float a1z = D1[2];
  float a2x = D1[3];
  float a2y = D1[4];
  float a2z = D1[5];
  float a3x = D1[6];
  float a3y = D1[7];
  float a3z = D1[8];
  float d1x = D1[9];
  float d1y = D1[10];
  float d1z = D1[11];
  float d2x = D1[12];
  float d2y = D1[13];
  float d2z = D1[14];
  float d3x = D1[15];
  float d3y = D1[16];
  float d3z = D1[17];

  float a4x = D2[0];
  float a4y = D2[1];
  float a4z = D2[2];
  float a5x = D2[3];
  float a5y = D2[4];
  float a5z = D2[5];
  float a6x = D2[6];
  float a6y = D2[7];
  float a6z = D2[8];
  float d4x = D2[9];
  float d4y = D2[10];
  float d4z = D2[11];
  float d5x = D2[12];
  float d5y = D2[13];
  float d5z = D2[14];
  float d6x = D2[15];
  float d6y = D2[16];
  float d6z = D2[17];

  float a7x = a1y * a4z - a1z * a4y;
  float a7y = a1z * a4x - a1x * a4z;
  float a7z = a1x * a4y - a1y * a4x;
  float a8x = a1y * a5z - a1z * a5y;
  float a8y = a1z * a5x - a1x * a5z;
  float a8z = a1x * a5y - a1y * a5x;
  float a9x = a1y * a6z - a1z * a6y;
  float a9y = a1z * a6x - a1x * a6z;
  float a9z = a1x * a6y - a1y * a6x;
  float aax = a2y * a4z - a2z * a4y;
  float aay = a2z * a4x - a2x * a4z;
  float aaz = a2x * a4y - a2y * a4x;
  float abx = a2y * a5z - a2z * a5y;
  float aby = a2z * a5x - a2x * a5z;
  float abz = a2x * a5y - a2y * a5x;
  float acx = a2y * a6z - a2z * a6y;
  float acy = a2z * a6x - a2x * a6z;
  float acz = a2x * a6y - a2y * a6x;
  float adx = a3y * a4z - a3z * a4y;
  float ady = a3z * a4x - a3x * a4z;
  float adz = a3x * a4y - a3y * a4x;
  float aex = a3y * a5z - a3z * a5y;
  float aey = a3z * a5x - a3x * a5z;
  float aez = a3x * a5y - a3y * a5x;
  float afx = a3y * a6z - a3z * a6y;
  float afy = a3z * a6x - a3x * a6z;
  float afz = a3x * a6y - a3y * a6x;
  // right or left flags
  bool right1, right2, right3;
  bool right4, right5, right6;
  bool right7, right8, right9;
  bool righta, rightb, rightc;
  bool rightd, righte, rightf;
  // overlapping distances
  float overlap1, overlap2, overlap3;
  float overlap4, overlap5, overlap6;
  float overlap7, overlap8, overlap9;
  float overlapa, overlapb, overlapc;
  float overlapd, overlape, overlapf;
  // invalid flags
  bool invalid7 = false;
  bool invalid8 = false;
  bool invalid9 = false;
  bool invalida = false;
  bool invalidb = false;
  bool invalidc = false;
  bool invalidd = false;
  bool invalide = false;
  bool invalidf = false;
  // temporary variables
  float len, len1, len2;
  float dot1, dot2, dot3;
  // try axis 1
  len    = a1x * dx + a1y * dy + a1z * dz;
  right1 = len > 0.f;
  if (!right1) {
    len = -len;
  }
  len1 = w1;
  dot1 = a1x * a4x + a1y * a4y + a1z * a4z;
  dot2 = a1x * a5x + a1y * a5y + a1z * a5z;
  dot3 = a1x * a6x + a1y * a6y + a1z * a6z;
  if (dot1 < 0.f) {
    dot1 = -dot1;
  }
  if (dot2 < 0.f) {
    dot2 = -dot2;
  }
  if (dot3 < 0.f) {
    dot3 = -dot3;
  }
  len2     = dot1 * w2 + dot2 * h2 + dot3 * d2;
  overlap1 = len - len1 - len2;
  if (overlap1 > 0.f) {
    return;
  }
  // try axis 2
  len    = a2x * dx + a2y * dy + a2z * dz;
  right2 = len > 0.f;
  if (!right2) {
    len = -len;
  }
  len1 = h1;
  dot1 = a2x * a4x + a2y * a4y + a2z * a4z;
  dot2 = a2x * a5x + a2y * a5y + a2z * a5z;
  dot3 = a2x * a6x + a2y * a6y + a2z * a6z;
  if (dot1 < 0.f) {
    dot1 = -dot1;
  }
  if (dot2 < 0.f) {
    dot2 = -dot2;
  }
  if (dot3 < 0.f) {
    dot3 = -dot3;
  }
  len2     = dot1 * w2 + dot2 * h2 + dot3 * d2;
  overlap2 = len - len1 - len2;
  if (overlap2 > 0.f) {
    return;
  }
  // try axis 3
  len    = a3x * dx + a3y * dy + a3z * dz;
  right3 = len > 0.f;
  if (!right3) {
    len = -len;
  }
  len1 = d1;
  dot1 = a3x * a4x + a3y * a4y + a3z * a4z;
  dot2 = a3x * a5x + a3y * a5y + a3z * a5z;
  dot3 = a3x * a6x + a3y * a6y + a3z * a6z;
  if (dot1 < 0.f) {
    dot1 = -dot1;
  }
  if (dot2 < 0.f) {
    dot2 = -dot2;
  }
  if (dot3 < 0.f) {
    dot3 = -dot3;
  }
  len2     = dot1 * w2 + dot2 * h2 + dot3 * d2;
  overlap3 = len - len1 - len2;
  if (overlap3 > 0.f) {
    return;
  }
  // try axis 4
  len    = a4x * dx + a4y * dy + a4z * dz;
  right4 = len > 0.f;
  if (!right4) {
    len = -len;
  }
  dot1 = a4x * a1x + a4y * a1y + a4z * a1z;
  dot2 = a4x * a2x + a4y * a2y + a4z * a2z;
  dot3 = a4x * a3x + a4y * a3y + a4z * a3z;
  if (dot1 < 0.f) {
    dot1 = -dot1;
  }
  if (dot2 < 0.f) {
    dot2 = -dot2;
  }
  if (dot3 < 0.f) {
    dot3 = -dot3;
  }
  len1     = dot1 * w1 + dot2 * h1 + dot3 * d1;
  len2     = w2;
  overlap4 = (len - len1 - len2) * 1.f;
  if (overlap4 > 0.f) {
    return;
  }
  // try axis 5
  len    = a5x * dx + a5y * dy + a5z * dz;
  right5 = len > 0.f;
  if (!right5) {
    len = -len;
  }
  dot1 = a5x * a1x + a5y * a1y + a5z * a1z;
  dot2 = a5x * a2x + a5y * a2y + a5z * a2z;
  dot3 = a5x * a3x + a5y * a3y + a5z * a3z;
  if (dot1 < 0.f) {
    dot1 = -dot1;
  }
  if (dot2 < 0.f) {
    dot2 = -dot2;
  }
  if (dot3 < 0.f) {
    dot3 = -dot3;
  }
  len1     = dot1 * w1 + dot2 * h1 + dot3 * d1;
  len2     = h2;
  overlap5 = (len - len1 - len2) * 1.f;
  if (overlap5 > 0.f) {
    return;
  }
  // try axis 6
  len    = a6x * dx + a6y * dy + a6z * dz;
  right6 = len > 0.f;
  if (!right6) {
    len = -len;
  }
  dot1 = a6x * a1x + a6y * a1y + a6z * a1z;
  dot2 = a6x * a2x + a6y * a2y + a6z * a2z;
  dot3 = a6x * a3x + a6y * a3y + a6z * a3z;
  if (dot1 < 0.f) {
    dot1 = -dot1;
  }
  if (dot2 < 0.f) {
    dot2 = -dot2;
  }
  if (dot3 < 0.f) {
    dot3 = -dot3;
  }
  len1     = dot1 * w1 + dot2 * h1 + dot3 * d1;
  len2     = d2;
  overlap6 = (len - len1 - len2) * 1.f;
  if (overlap6 > 0.f) {
    return;
  }
  // try axis 7
  len = a7x * a7x + a7y * a7y + a7z * a7z;
  if (len > 1e-5f) {
    len = 1.f / std::sqrt(len);
    a7x *= len;
    a7y *= len;
    a7z *= len;
    len    = a7x * dx + a7y * dy + a7z * dz;
    right7 = len > 0.f;
    if (!right7) {
      len = -len;
    }
    dot1 = a7x * a2x + a7y * a2y + a7z * a2z;
    dot2 = a7x * a3x + a7y * a3y + a7z * a3z;
    if (dot1 < 0.f) {
      dot1 = -dot1;
    }
    if (dot2 < 0.f) {
      dot2 = -dot2;
    }
    len1 = dot1 * h1 + dot2 * d1;
    dot1 = a7x * a5x + a7y * a5y + a7z * a5z;
    dot2 = a7x * a6x + a7y * a6y + a7z * a6z;
    if (dot1 < 0.f) {
      dot1 = -dot1;
    }
    if (dot2 < 0.f) {
      dot2 = -dot2;
    }
    len2     = dot1 * h2 + dot2 * d2;
    overlap7 = len - len1 - len2;
    if (overlap7 > 0.f) {
      return;
    }
  }
  else {
    right7   = false;
    overlap7 = 0.f;
    invalid7 = true;
  }
  // try axis 8
  len = a8x * a8x + a8y * a8y + a8z * a8z;
  if (len > 1e-5f) {
    len = 1.f / std::sqrt(len);
    a8x *= len;
    a8y *= len;
    a8z *= len;
    len    = a8x * dx + a8y * dy + a8z * dz;
    right8 = len > 0.f;
    if (!right8) {
      len = -len;
    }
    dot1 = a8x * a2x + a8y * a2y + a8z * a2z;
    dot2 = a8x * a3x + a8y * a3y + a8z * a3z;
    if (dot1 < 0.f) {
      dot1 = -dot1;
    }
    if (dot2 < 0.f) {
      dot2 = -dot2;
    }
    len1 = dot1 * h1 + dot2 * d1;
    dot1 = a8x * a4x + a8y * a4y + a8z * a4z;
    dot2 = a8x * a6x + a8y * a6y + a8z * a6z;
    if (dot1 < 0.f) {
      dot1 = -dot1;
    }
    if (dot2 < 0.f) {
      dot2 = -dot2;
    }
    len2     = dot1 * w2 + dot2 * d2;
    overlap8 = len - len1 - len2;
    if (overlap8 > 0.f) {
      return;
    }
  }
  else {
    right8   = false;
    overlap8 = 0.f;
    invalid8 = true;
  }
  // try axis 9
  len = a9x * a9x + a9y * a9y + a9z * a9z;
  if (len > 1e-5f) {
    len = 1.f / std::sqrt(len);
    a9x *= len;
    a9y *= len;
    a9z *= len;
    len    = a9x * dx + a9y * dy + a9z * dz;
    right9 = len > 0.f;
    if (!right9) {
      len = -len;
    }
    dot1 = a9x * a2x + a9y * a2y + a9z * a2z;
    dot2 = a9x * a3x + a9y * a3y + a9z * a3z;
    if (dot1 < 0.f) {
      dot1 = -dot1;
    }
    if (dot2 < 0.f) {
      dot2 = -dot2;
    }
    len1 = dot1 * h1 + dot2 * d1;
    dot1 = a9x * a4x + a9y * a4y + a9z * a4z;
    dot2 = a9x * a5x + a9y * a5y + a9z * a5z;
    if (dot1 < 0.f) {
      dot1 = -dot1;
    }
    if (dot2 < 0.f) {
      dot2 = -dot2;
    }
    len2     = dot1 * w2 + dot2 * h2;
    overlap9 = len - len1 - len2;
    if (overlap9 > 0.f) {
      return;
    }
  }
  else {
    right9   = false;
    overlap9 = 0.f;
    invalid9 = true;
  }
  // try axis 10
  len = aax * aax + aay * aay + aaz * aaz;
  if (len > 1e-5f) {
    len = 1.f / std::sqrt(len);
    aax *= len;
    aay *= len;
    aaz *= len;
    len    = aax * dx + aay * dy + aaz * dz;
    righta = len > 0.f;
    if (!righta) {
      len = -len;
    }
    dot1 = aax * a1x + aay * a1y + aaz * a1z;
    dot2 = aax * a3x + aay * a3y + aaz * a3z;
    if (dot1 < 0.f) {
      dot1 = -dot1;
    }
    if (dot2 < 0.f) {
      dot2 = -dot2;
    }
    len1 = dot1 * w1 + dot2 * d1;
    dot1 = aax * a5x + aay * a5y + aaz * a5z;
    dot2 = aax * a6x + aay * a6y + aaz * a6z;
    if (dot1 < 0.f) {
      dot1 = -dot1;
    }
    if (dot2 < 0.f) {
      dot2 = -dot2;
    }
    len2     = dot1 * h2 + dot2 * d2;
    overlapa = len - len1 - len2;
    if (overlapa > 0.f) {
      return;
    }
  }
  else {
    righta   = false;
    overlapa = 0.f;
    invalida = true;
  }
  // try axis 11
  len = abx * abx + aby * aby + abz * abz;
  if (len > 1e-5f) {
    len = 1.f / std::sqrt(len);
    abx *= len;
    aby *= len;
    abz *= len;
    len    = abx * dx + aby * dy + abz * dz;
    rightb = len > 0.f;
    if (!rightb) {
      len = -len;
    }
    dot1 = abx * a1x + aby * a1y + abz * a1z;
    dot2 = abx * a3x + aby * a3y + abz * a3z;
    if (dot1 < 0.f) {
      dot1 = -dot1;
    }
    if (dot2 < 0.f) {
      dot2 = -dot2;
    }
    len1 = dot1 * w1 + dot2 * d1;
    dot1 = abx * a4x + aby * a4y + abz * a4z;
    dot2 = abx * a6x + aby * a6y + abz * a6z;
    if (dot1 < 0.f) {
      dot1 = -dot1;
    }
    if (dot2 < 0.f) {
      dot2 = -dot2;
    }
    len2     = dot1 * w2 + dot2 * d2;
    overlapb = len - len1 - len2;
    if (overlapb > 0.f) {
      return;
    }
  }
  else {
    rightb   = false;
    overlapb = 0.f;
    invalidb = true;
  }
  // try axis 12
  len = acx * acx + acy * acy + acz * acz;
  if (len > 1e-5f) {
    len = 1.f / std::sqrt(len);
    acx *= len;
    acy *= len;
    acz *= len;
    len    = acx * dx + acy * dy + acz * dz;
    rightc = len > 0.f;
    if (!rightc) {
      len = -len;
    }
    dot1 = acx * a1x + acy * a1y + acz * a1z;
    dot2 = acx * a3x + acy * a3y + acz * a3z;
    if (dot1 < 0.f) {
      dot1 = -dot1;
    }
    if (dot2 < 0.f) {
      dot2 = -dot2;
    }
    len1 = dot1 * w1 + dot2 * d1;
    dot1 = acx * a4x + acy * a4y + acz * a4z;
    dot2 = acx * a5x + acy * a5y + acz * a5z;
    if (dot1 < 0.f) {
      dot1 = -dot1;
    }
    if (dot2 < 0.f) {
      dot2 = -dot2;
    }
    len2     = dot1 * w2 + dot2 * h2;
    overlapc = len - len1 - len2;
    if (overlapc > 0.f) {
      return;
    }
  }
  else {
    rightc   = false;
    overlapc = 0.f;
    invalidc = true;
  }
  // try axis 13
  len = adx * adx + ady * ady + adz * adz;
  if (len > 1e-5f) {
    len = 1.f / std::sqrt(len);
    adx *= len;
    ady *= len;
    adz *= len;
    len    = adx * dx + ady * dy + adz * dz;
    rightd = len > 0.f;
    if (!rightd) {
      len = -len;
    }
    dot1 = adx * a1x + ady * a1y + adz * a1z;
    dot2 = adx * a2x + ady * a2y + adz * a2z;
    if (dot1 < 0.f) {
      dot1 = -dot1;
    }
    if (dot2 < 0.f) {
      dot2 = -dot2;
    }
    len1 = dot1 * w1 + dot2 * h1;
    dot1 = adx * a5x + ady * a5y + adz * a5z;
    dot2 = adx * a6x + ady * a6y + adz * a6z;
    if (dot1 < 0.f) {
      dot1 = -dot1;
    }
    if (dot2 < 0.f) {
      dot2 = -dot2;
    }
    len2     = dot1 * h2 + dot2 * d2;
    overlapd = len - len1 - len2;
    if (overlapd > 0.f) {
      return;
    }
  }
  else {
    rightd   = false;
    overlapd = 0.f;
    invalidd = true;
  }
  // try axis 14
  len = aex * aex + aey * aey + aez * aez;
  if (len > 1e-5f) {
    len = 1.f / std::sqrt(len);
    aex *= len;
    aey *= len;
    aez *= len;
    len    = aex * dx + aey * dy + aez * dz;
    righte = len > 0.f;
    if (!righte) {
      len = -len;
    }
    dot1 = aex * a1x + aey * a1y + aez * a1z;
    dot2 = aex * a2x + aey * a2y + aez * a2z;
    if (dot1 < 0.f) {
      dot1 = -dot1;
    }
    if (dot2 < 0.f) {
      dot2 = -dot2;
    }
    len1 = dot1 * w1 + dot2 * h1;
    dot1 = aex * a4x + aey * a4y + aez * a4z;
    dot2 = aex * a6x + aey * a6y + aez * a6z;
    if (dot1 < 0.f) {
      dot1 = -dot1;
    }
    if (dot2 < 0.f) {
      dot2 = -dot2;
    }
    len2     = dot1 * w2 + dot2 * d2;
    overlape = len - len1 - len2;
    if (overlape > 0.f) {
      return;
    }
  }
  else {
    righte   = false;
    overlape = 0;
    invalide = true;
  }
  // try axis 15
  len = afx * afx + afy * afy + afz * afz;
  if (len > 1e-5f) {
    len = 1.f / std::sqrt(len);
    afx *= len;
    afy *= len;
    afz *= len;
    len    = afx * dx + afy * dy + afz * dz;
    rightf = len > 0.f;
    if (!rightf) {
      len = -len;
    }
    dot1 = afx * a1x + afy * a1y + afz * a1z;
    dot2 = afx * a2x + afy * a2y + afz * a2z;
    if (dot1 < 0.f) {
      dot1 = -dot1;
    }
    if (dot2 < 0.f) {
      dot2 = -dot2;
    }
    len1 = dot1 * w1 + dot2 * h1;
    dot1 = afx * a4x + afy * a4y + afz * a4z;
    dot2 = afx * a5x + afy * a5y + afz * a5z;
    if (dot1 < 0.f) {
      dot1 = -dot1;
    }
    if (dot2 < 0.f) {
      dot2 = -dot2;
    }
    len2     = dot1 * w2 + dot2 * h2;
    overlapf = len - len1 - len2;
    if (overlapf > 0.f) {
      return;
    }
  }
  else {
    rightf   = false;
    overlapf = 0;
    invalidf = true;
  }
  // boxes are overlapping
  float depth           = overlap1;
  float depth2          = overlap1;
  unsigned int minIndex = 0;
  bool right            = right1;
  if (overlap2 > depth2) {
    depth    = overlap2;
    depth2   = overlap2;
    minIndex = 1;
    right    = right2;
  }
  if (overlap3 > depth2) {
    depth    = overlap3;
    depth2   = overlap3;
    minIndex = 2;
    right    = right3;
  }
  if (overlap4 > depth2) {
    depth    = overlap4;
    depth2   = overlap4;
    minIndex = 3;
    right    = right4;
  }
  if (overlap5 > depth2) {
    depth    = overlap5;
    depth2   = overlap5;
    minIndex = 4;
    right    = right5;
  }
  if (overlap6 > depth2) {
    depth    = overlap6;
    depth2   = overlap6;
    minIndex = 5;
    right    = right6;
  }
  if (overlap7 - 0.01f > depth2 && !invalid7) {
    depth    = overlap7;
    depth2   = overlap7 - 0.01f;
    minIndex = 6;
    right    = right7;
  }
  if (overlap8 - 0.01f > depth2 && !invalid8) {
    depth    = overlap8;
    depth2   = overlap8 - 0.01f;
    minIndex = 7;
    right    = right8;
  }
  if (overlap9 - 0.01f > depth2 && !invalid9) {
    depth    = overlap9;
    depth2   = overlap9 - 0.01f;
    minIndex = 8;
    right    = right9;
  }
  if (overlapa - 0.01f > depth2 && !invalida) {
    depth    = overlapa;
    depth2   = overlapa - 0.01f;
    minIndex = 9;
    right    = righta;
  }
  if (overlapb - 0.01f > depth2 && !invalidb) {
    depth    = overlapb;
    depth2   = overlapb - 0.01f;
    minIndex = 10;
    right    = rightb;
  }
  if (overlapc - 0.01f > depth2 && !invalidc) {
    depth    = overlapc;
    depth2   = overlapc - 0.01f;
    minIndex = 11;
    right    = rightc;
  }
  if (overlapd - 0.01f > depth2 && !invalidd) {
    depth    = overlapd;
    depth2   = overlapd - 0.01f;
    minIndex = 12;
    right    = rightd;
  }
  if (overlape - 0.01f > depth2 && !invalide) {
    depth    = overlape;
    depth2   = overlape - 0.01f;
    minIndex = 13;
    right    = righte;
  }
  if (overlapf - 0.01f > depth2 && !invalidf) {
    depth    = overlapf;
    minIndex = 14;
    right    = rightf;
  }
  // normal
  float nx = 0.f;
  float ny = 0.f;
  float nz = 0.f;
  // edge line or face side normal
  float n1x = 0.f;
  float n1y = 0.f;
  float n1z = 0.f;
  float n2x = 0.f;
  float n2y = 0.f;
  float n2z = 0.f;
  // center of current face
  float cx = 0.f;
  float cy = 0.f;
  float cz = 0.f;
  // face side
  float s1x = 0.f;
  float s1y = 0.f;
  float s1z = 0.f;
  float s2x = 0.f;
  float s2y = 0.f;
  float s2z = 0.f;
  // swap b1 b2
  bool swap = false;

  //----------------------------------------------------------------------------

  if (minIndex == 0) { // b1.x * b2
    if (right) {
      cx = p1x + d1x;
      cy = p1y + d1y;
      cz = p1z + d1z;
      nx = a1x;
      ny = a1y;
      nz = a1z;
    }
    else {
      cx = p1x - d1x;
      cy = p1y - d1y;
      cz = p1z - d1z;
      nx = -a1x;
      ny = -a1y;
      nz = -a1z;
    }
    s1x = d2x;
    s1y = d2y;
    s1z = d2z;
    n1x = -a2x;
    n1y = -a2y;
    n1z = -a2z;
    s2x = d3x;
    s2y = d3y;
    s2z = d3z;
    n2x = -a3x;
    n2y = -a3y;
    n2z = -a3z;
  }
  else if (minIndex == 1) { // b1.y * b2
    if (right) {
      cx = p1x + d2x;
      cy = p1y + d2y;
      cz = p1z + d2z;
      nx = a2x;
      ny = a2y;
      nz = a2z;
    }
    else {
      cx = p1x - d2x;
      cy = p1y - d2y;
      cz = p1z - d2z;
      nx = -a2x;
      ny = -a2y;
      nz = -a2z;
    }
    s1x = d1x;
    s1y = d1y;
    s1z = d1z;
    n1x = -a1x;
    n1y = -a1y;
    n1z = -a1z;
    s2x = d3x;
    s2y = d3y;
    s2z = d3z;
    n2x = -a3x;
    n2y = -a3y;
    n2z = -a3z;
  }
  else if (minIndex == 2) { // b1.z * b2
    if (right) {
      cx = p1x + d3x;
      cy = p1y + d3y;
      cz = p1z + d3z;
      nx = a3x;
      ny = a3y;
      nz = a3z;
    }
    else {
      cx = p1x - d3x;
      cy = p1y - d3y;
      cz = p1z - d3z;
      nx = -a3x;
      ny = -a3y;
      nz = -a3z;
    }
    s1x = d1x;
    s1y = d1y;
    s1z = d1z;
    n1x = -a1x;
    n1y = -a1y;
    n1z = -a1z;
    s2x = d2x;
    s2y = d2y;
    s2z = d2z;
    n2x = -a2x;
    n2y = -a2y;
    n2z = -a2z;
  }
  else if (minIndex == 3) { // b2.x * b1
    swap = true;
    if (!right) {
      cx = p2x + d4x;
      cy = p2y + d4y;
      cz = p2z + d4z;
      nx = a4x;
      ny = a4y;
      nz = a4z;
    }
    else {
      cx = p2x - d4x;
      cy = p2y - d4y;
      cz = p2z - d4z;
      nx = -a4x;
      ny = -a4y;
      nz = -a4z;
    }
    s1x = d5x;
    s1y = d5y;
    s1z = d5z;
    n1x = -a5x;
    n1y = -a5y;
    n1z = -a5z;
    s2x = d6x;
    s2y = d6y;
    s2z = d6z;
    n2x = -a6x;
    n2y = -a6y;
    n2z = -a6z;
  }
  else if (minIndex == 4) { // b2.y * b1
    swap = true;
    if (!right) {
      cx = p2x + d5x;
      cy = p2y + d5y;
      cz = p2z + d5z;
      nx = a5x;
      ny = a5y;
      nz = a5z;
    }
    else {
      cx = p2x - d5x;
      cy = p2y - d5y;
      cz = p2z - d5z;
      nx = -a5x;
      ny = -a5y;
      nz = -a5z;
    }
    s1x = d4x;
    s1y = d4y;
    s1z = d4z;
    n1x = -a4x;
    n1y = -a4y;
    n1z = -a4z;
    s2x = d6x;
    s2y = d6y;
    s2z = d6z;
    n2x = -a6x;
    n2y = -a6y;
    n2z = -a6z;
  }
  else if (minIndex == 5) { // b2.z * b1
    swap = true;
    if (!right) {
      cx = p2x + d6x;
      cy = p2y + d6y;
      cz = p2z + d6z;
      nx = a6x;
      ny = a6y;
      nz = a6z;
    }
    else {
      cx = p2x - d6x;
      cy = p2y - d6y;
      cz = p2z - d6z;
      nx = -a6x;
      ny = -a6y;
      nz = -a6z;
    }
    s1x = d4x;
    s1y = d4y;
    s1z = d4z;
    n1x = -a4x;
    n1y = -a4y;
    n1z = -a4z;
    s2x = d5x;
    s2y = d5y;
    s2z = d5z;
    n2x = -a5x;
    n2y = -a5y;
    n2z = -a5z;
  }
  else if (minIndex == 6) { // b1.x * b2.x
    nx  = a7x;
    ny  = a7y;
    nz  = a7z;
    n1x = a1x;
    n1y = a1y;
    n1z = a1z;
    n2x = a4x;
    n2y = a4y;
    n2z = a4z;
  }
  else if (minIndex == 7) { // b1.x * b2.y
    nx  = a8x;
    ny  = a8y;
    nz  = a8z;
    n1x = a1x;
    n1y = a1y;
    n1z = a1z;
    n2x = a5x;
    n2y = a5y;
    n2z = a5z;
  }
  else if (minIndex == 8) { // b1.x * b2.z
    nx  = a9x;
    ny  = a9y;
    nz  = a9z;
    n1x = a1x;
    n1y = a1y;
    n1z = a1z;
    n2x = a6x;
    n2y = a6y;
    n2z = a6z;
  }
  else if (minIndex == 9) { // b1.y * b2.x
    nx  = aax;
    ny  = aay;
    nz  = aaz;
    n1x = a2x;
    n1y = a2y;
    n1z = a2z;
    n2x = a4x;
    n2y = a4y;
    n2z = a4z;
  }
  else if (minIndex == 10) { // b1.y * b2.y
    nx  = abx;
    ny  = aby;
    nz  = abz;
    n1x = a2x;
    n1y = a2y;
    n1z = a2z;
    n2x = a5x;
    n2y = a5y;
    n2z = a5z;
  }
  else if (minIndex == 11) { // b1.y * b2.z
    nx  = acx;
    ny  = acy;
    nz  = acz;
    n1x = a2x;
    n1y = a2y;
    n1z = a2z;
    n2x = a6x;
    n2y = a6y;
    n2z = a6z;
  }
  else if (minIndex == 12) { // b1.z * b2.x
    nx  = adx;
    ny  = ady;
    nz  = adz;
    n1x = a3x;
    n1y = a3y;
    n1z = a3z;
    n2x = a4x;
    n2y = a4y;
    n2z = a4z;
  }
  else if (minIndex == 13) { // b1.z * b2.y
    nx  = aex;
    ny  = aey;
    nz  = aez;
    n1x = a3x;
    n1y = a3y;
    n1z = a3z;
    n2x = a5x;
    n2y = a5y;
    n2z = a5z;
  }
  else if (minIndex == 14) { // b1.z * b2.z
    nx  = afx;
    ny  = afy;
    nz  = afz;
    n1x = a3x;
    n1y = a3y;
    n1z = a3z;
    n2x = a6x;
    n2y = a6y;
    n2z = a6z;
  }

  //----------------------------------------------------------------------------

  if (minIndex > 5) {
    if (!right) {
      nx = -nx;
      ny = -ny;
      nz = -nz;
    }
    float distance;
    float maxDistance;
    float vx, vy, vz;
    float v1x, v1y, v1z;
    float v2x, v2y, v2z;
    // vertex1;
    v1x         = V1[0];
    v1y         = V1[1];
    v1z         = V1[2];
    maxDistance = nx * v1x + ny * v1y + nz * v1z;
    // vertex2;
    vx       = V1[3];
    vy       = V1[4];
    vz       = V1[5];
    distance = nx * vx + ny * vy + nz * vz;
    if (distance > maxDistance) {
      maxDistance = distance;
      v1x         = vx;
      v1y         = vy;
      v1z         = vz;
    }
    // vertex3;
    vx       = V1[6];
    vy       = V1[7];
    vz       = V1[8];
    distance = nx * vx + ny * vy + nz * vz;
    if (distance > maxDistance) {
      maxDistance = distance;
      v1x         = vx;
      v1y         = vy;
      v1z         = vz;
    }
    // vertex4;
    vx       = V1[9];
    vy       = V1[10];
    vz       = V1[11];
    distance = nx * vx + ny * vy + nz * vz;
    if (distance > maxDistance) {
      maxDistance = distance;
      v1x         = vx;
      v1y         = vy;
      v1z         = vz;
    }
    // vertex5;
    vx       = V1[12];
    vy       = V1[13];
    vz       = V1[14];
    distance = nx * vx + ny * vy + nz * vz;
    if (distance > maxDistance) {
      maxDistance = distance;
      v1x         = vx;
      v1y         = vy;
      v1z         = vz;
    }
    // vertex6;
    vx       = V1[15];
    vy       = V1[16];
    vz       = V1[17];
    distance = nx * vx + ny * vy + nz * vz;
    if (distance > maxDistance) {
      maxDistance = distance;
      v1x         = vx;
      v1y         = vy;
      v1z         = vz;
    }
    // vertex7;
    vx       = V1[18];
    vy       = V1[19];
    vz       = V1[20];
    distance = nx * vx + ny * vy + nz * vz;
    if (distance > maxDistance) {
      maxDistance = distance;
      v1x         = vx;
      v1y         = vy;
      v1z         = vz;
    }
    // vertex8;
    vx       = V1[21];
    vy       = V1[22];
    vz       = V1[23];
    distance = nx * vx + ny * vy + nz * vz;
    if (distance > maxDistance) {
      maxDistance = distance;
      v1x         = vx;
      v1y         = vy;
      v1z         = vz;
    }
    // vertex1;
    v2x         = V2[0];
    v2y         = V2[1];
    v2z         = V2[2];
    maxDistance = nx * v2x + ny * v2y + nz * v2z;
    // vertex2;
    vx       = V2[3];
    vy       = V2[4];
    vz       = V2[5];
    distance = nx * vx + ny * vy + nz * vz;
    if (distance < maxDistance) {
      maxDistance = distance;
      v2x         = vx;
      v2y         = vy;
      v2z         = vz;
    }
    // vertex3;
    vx       = V2[6];
    vy       = V2[7];
    vz       = V2[8];
    distance = nx * vx + ny * vy + nz * vz;
    if (distance < maxDistance) {
      maxDistance = distance;
      v2x         = vx;
      v2y         = vy;
      v2z         = vz;
    }
    // vertex4;
    vx       = V2[9];
    vy       = V2[10];
    vz       = V2[11];
    distance = nx * vx + ny * vy + nz * vz;
    if (distance < maxDistance) {
      maxDistance = distance;
      v2x         = vx;
      v2y         = vy;
      v2z         = vz;
    }
    // vertex5;
    vx       = V2[12];
    vy       = V2[13];
    vz       = V2[14];
    distance = nx * vx + ny * vy + nz * vz;
    if (distance < maxDistance) {
      maxDistance = distance;
      v2x         = vx;
      v2y         = vy;
      v2z         = vz;
    }
    // vertex6;
    vx       = V2[15];
    vy       = V2[16];
    vz       = V2[17];
    distance = nx * vx + ny * vy + nz * vz;
    if (distance < maxDistance) {
      maxDistance = distance;
      v2x         = vx;
      v2y         = vy;
      v2z         = vz;
    }
    // vertex7;
    vx       = V2[18];
    vy       = V2[19];
    vz       = V2[20];
    distance = nx * vx + ny * vy + nz * vz;
    if (distance < maxDistance) {
      maxDistance = distance;
      v2x         = vx;
      v2y         = vy;
      v2z         = vz;
    }
    // vertex8;
    vx       = V2[21];
    vy       = V2[22];
    vz       = V2[23];
    distance = nx * vx + ny * vy + nz * vz;
    if (distance < maxDistance) {
      maxDistance = distance;
      v2x         = vx;
      v2y         = vy;
      v2z         = vz;
    }
    vx      = v2x - v1x;
    vy      = v2y - v1y;
    vz      = v2z - v1z;
    dot1    = n1x * n2x + n1y * n2y + n1z * n2z;
    float t = (vx * (n1x - n2x * dot1) + vy * (n1y - n2y * dot1)
               + vz * (n1z - n2z * dot1))
              / (1.f - dot1 * dot1);
    manifold->addPoint(
      v1x + n1x * t + nx * depth * 0.5f, v1y + n1y * t + ny * depth * 0.5f,
      v1z + n1z * t + nz * depth * 0.5f, nx, ny, nz, depth, false);
    return;
  }
  // now detect face-face collision...
  // target quad
  float q1x = 0.f, q1y = 0.f, q1z = 0.f;
  float q2x = 0.f, q2y = 0.f, q2z = 0.f;
  float q3x = 0.f, q3y = 0.f, q3z = 0.f;
  float q4x = 0.f, q4y = 0.f, q4z = 0.f;
  // search support face and vertex
  float minDot             = 1.f;
  float dot                = 0.f;
  unsigned int minDotIndex = 0;
  if (swap) {
    dot = a1x * nx + a1y * ny + a1z * nz;
    if (dot < minDot) {
      minDot      = dot;
      minDotIndex = 0;
    }
    if (-dot < minDot) {
      minDot      = -dot;
      minDotIndex = 1;
    }
    dot = a2x * nx + a2y * ny + a2z * nz;
    if (dot < minDot) {
      minDot      = dot;
      minDotIndex = 2;
    }
    if (-dot < minDot) {
      minDot      = -dot;
      minDotIndex = 3;
    }
    dot = a3x * nx + a3y * ny + a3z * nz;
    if (dot < minDot) {
      minDot      = dot;
      minDotIndex = 4;
    }
    if (-dot < minDot) {
      minDot      = -dot;
      minDotIndex = 5;
    }

    if (minDotIndex == 0) { // x+ face
      q1x = V1[0];
      q1y = V1[1];
      q1z = V1[2]; // vertex1
      q2x = V1[6];
      q2y = V1[7];
      q2z = V1[8]; // vertex3
      q3x = V1[9];
      q3y = V1[10];
      q3z = V1[11]; // vertex4
      q4x = V1[3];
      q4y = V1[4];
      q4z = V1[5]; // vertex2
    }
    else if (minDotIndex == 1) { // x- face
      q1x = V1[15];
      q1y = V1[16];
      q1z = V1[17]; // vertex6
      q2x = V1[21];
      q2y = V1[22];
      q2z = V1[23]; // vertex8
      q3x = V1[18];
      q3y = V1[19];
      q3z = V1[20]; // vertex7
      q4x = V1[12];
      q4y = V1[13];
      q4z = V1[14]; // vertex5
    }
    else if (minDotIndex == 2) { // y+ face
      q1x = V1[12];
      q1y = V1[13];
      q1z = V1[14]; // vertex5
      q2x = V1[0];
      q2y = V1[1];
      q2z = V1[2]; // vertex1
      q3x = V1[3];
      q3y = V1[4];
      q3z = V1[5]; // vertex2
      q4x = V1[15];
      q4y = V1[16];
      q4z = V1[17]; // vertex6
    }
    else if (minDotIndex == 3) { // y- face
      q1x = V1[21];
      q1y = V1[22];
      q1z = V1[23]; // vertex8
      q2x = V1[9];
      q2y = V1[10];
      q2z = V1[11]; // vertex4
      q3x = V1[6];
      q3y = V1[7];
      q3z = V1[8]; // vertex3
      q4x = V1[18];
      q4y = V1[19];
      q4z = V1[20]; // vertex7
    }
    else if (minDotIndex == 4) { // z+ face
      q1x = V1[12];
      q1y = V1[13];
      q1z = V1[14]; // vertex5
      q2x = V1[18];
      q2y = V1[19];
      q2z = V1[20]; // vertex7
      q3x = V1[6];
      q3y = V1[7];
      q3z = V1[8]; // vertex3
      q4x = V1[0];
      q4y = V1[1];
      q4z = V1[2]; // vertex1
    }
    else if (minDotIndex == 5) { // z- face
      q1x = V1[3];
      q1y = V1[4];
      q1z = V1[5]; // vertex2
      q2x = V1[6];
      q2y = V1[7];
      q2z = V1[8]; // vertex4
      q3x = V1[21];
      q3y = V1[22];
      q3z = V1[23]; // vertex8
      q4x = V1[15];
      q4y = V1[16];
      q4z = V1[17]; // vertex6
    }
  }
  else {
    dot = a4x * nx + a4y * ny + a4z * nz;
    if (dot < minDot) {
      minDot      = dot;
      minDotIndex = 0;
    }
    if (-dot < minDot) {
      minDot      = -dot;
      minDotIndex = 1;
    }
    dot = a5x * nx + a5y * ny + a5z * nz;
    if (dot < minDot) {
      minDot      = dot;
      minDotIndex = 2;
    }
    if (-dot < minDot) {
      minDot      = -dot;
      minDotIndex = 3;
    }
    dot = a6x * nx + a6y * ny + a6z * nz;
    if (dot < minDot) {
      minDot      = dot;
      minDotIndex = 4;
    }
    if (-dot < minDot) {
      minDot      = -dot;
      minDotIndex = 5;
    }

    //--------------------------------------------------------------------------

    if (minDotIndex == 0) { // x+ face
      q1x = V2[0];
      q1y = V2[1];
      q1z = V2[2]; // vertex1
      q2x = V2[6];
      q2y = V2[7];
      q2z = V2[8]; // vertex3
      q3x = V2[9];
      q3y = V2[10];
      q3z = V2[11]; // vertex4
      q4x = V2[3];
      q4y = V2[4];
      q4z = V2[5]; // vertex2
    }
    else if (minDotIndex == 1) { // x- face
      q1x = V2[15];
      q1y = V2[16];
      q1z = V2[17]; // vertex6
      q2x = V2[21];
      q2y = V2[22];
      q2z = V2[23]; // vertex8
      q3x = V2[18];
      q3y = V2[19];
      q3z = V2[20]; // vertex7
      q4x = V2[12];
      q4y = V2[13];
      q4z = V2[14]; // vertex5
    }
    else if (minDotIndex == 2) { // y+ face
      q1x = V2[12];
      q1y = V2[13];
      q1z = V2[14]; // vertex5
      q2x = V2[0];
      q2y = V2[1];
      q2z = V2[2]; // vertex1
      q3x = V2[3];
      q3y = V2[4];
      q3z = V2[5]; // vertex2
      q4x = V2[15];
      q4y = V2[16];
      q4z = V2[17]; // vertex6
    }
    else if (minDotIndex == 3) { // y- face
      q1x = V2[21];
      q1y = V2[22];
      q1z = V2[23]; // vertex8
      q2x = V2[9];
      q2y = V2[10];
      q2z = V2[11]; // vertex4
      q3x = V2[6];
      q3y = V2[7];
      q3z = V2[8]; // vertex3
      q4x = V2[18];
      q4y = V2[19];
      q4z = V2[20]; // vertex7
    }
    else if (minDotIndex == 4) { // z+ face
      q1x = V2[12];
      q1y = V2[13];
      q1z = V2[14]; // vertex5
      q2x = V2[18];
      q2y = V2[19];
      q2z = V2[20]; // vertex7
      q3x = V2[6];
      q3y = V2[7];
      q3z = V2[8]; // vertex3
      q4x = V2[0];
      q4y = V2[1];
      q4z = V2[2]; // vertex1
    }
    else if (minDotIndex == 5) { // z- face
      q1x = V2[3];
      q1y = V2[4];
      q1z = V2[5]; // vertex2
      q2x = V2[9];
      q2y = V2[10];
      q2z = V2[11]; // vertex4
      q3x = V2[21];
      q3y = V2[22];
      q3z = V2[23]; // vertex8
      q4x = V2[15];
      q4y = V2[16];
      q4z = V2[17]; // vertex6
    }
  }
  // clip vertices
  unsigned int numClipVertices;
  unsigned int numAddedClipVertices;
  unsigned int index;
  float x1, y1, z1;
  float x2, y2, z2;
  float t;
  _clipVertices1[0]    = q1x;
  _clipVertices1[1]    = q1y;
  _clipVertices1[2]    = q1z;
  _clipVertices1[3]    = q2x;
  _clipVertices1[4]    = q2y;
  _clipVertices1[5]    = q2z;
  _clipVertices1[6]    = q3x;
  _clipVertices1[7]    = q3y;
  _clipVertices1[8]    = q3z;
  _clipVertices1[9]    = q4x;
  _clipVertices1[10]   = q4y;
  _clipVertices1[11]   = q4z;
  numAddedClipVertices = 0;
  x1                   = _clipVertices1[9];
  y1                   = _clipVertices1[10];
  z1                   = _clipVertices1[11];
  dot1 = (x1 - cx - s1x) * n1x + (y1 - cy - s1y) * n1y + (z1 - cz - s1z) * n1z;

  for (unsigned int i = 0; i < 4; ++i) {
    index = i * 3;
    x2    = _clipVertices1[index];
    y2    = _clipVertices1[index + 1];
    z2    = _clipVertices1[index + 2];
    dot2
      = (x2 - cx - s1x) * n1x + (y2 - cy - s1y) * n1y + (z2 - cz - s1z) * n1z;
    if (dot1 > 0.f) {
      if (dot2 > 0.f) {
        index = numAddedClipVertices * 3;
        ++numAddedClipVertices;
        _clipVertices2[index]     = x2;
        _clipVertices2[index + 1] = y2;
        _clipVertices2[index + 2] = z2;
      }
      else {
        index = numAddedClipVertices * 3;
        ++numAddedClipVertices;
        t                         = dot1 / (dot1 - dot2);
        _clipVertices2[index]     = x1 + (x2 - x1) * t;
        _clipVertices2[index + 1] = y1 + (y2 - y1) * t;
        _clipVertices2[index + 2] = z1 + (z2 - z1) * t;
      }
    }
    else {
      if (dot2 > 0.f) {
        index = numAddedClipVertices * 3;
        ++numAddedClipVertices;
        t                         = dot1 / (dot1 - dot2);
        _clipVertices2[index]     = x1 + (x2 - x1) * t;
        _clipVertices2[index + 1] = y1 + (y2 - y1) * t;
        _clipVertices2[index + 2] = z1 + (z2 - z1) * t;
        index                     = numAddedClipVertices * 3;
        ++numAddedClipVertices;
        _clipVertices2[index]     = x2;
        _clipVertices2[index + 1] = y2;
        _clipVertices2[index + 2] = z2;
      }
    }
    x1   = x2;
    y1   = y2;
    z1   = z2;
    dot1 = dot2;
  }

  numClipVertices = numAddedClipVertices;
  if (numClipVertices == 0) {
    return;
  }
  numAddedClipVertices = 0;
  index                = (numClipVertices - 1) * 3;
  x1                   = _clipVertices2[index];
  y1                   = _clipVertices2[index + 1];
  z1                   = _clipVertices2[index + 2];
  dot1 = (x1 - cx - s2x) * n2x + (y1 - cy - s2y) * n2y + (z1 - cz - s2z) * n2z;

  for (unsigned int i = 0; i < numClipVertices; ++i) {
    index = i * 3;
    x2    = _clipVertices2[index];
    y2    = _clipVertices2[index + 1];
    z2    = _clipVertices2[index + 2];
    dot2
      = (x2 - cx - s2x) * n2x + (y2 - cy - s2y) * n2y + (z2 - cz - s2z) * n2z;
    if (dot1 > 0.f) {
      if (dot2 > 0.f) {
        index = numAddedClipVertices * 3;
        ++numAddedClipVertices;
        _clipVertices1[index]     = x2;
        _clipVertices1[index + 1] = y2;
        _clipVertices1[index + 2] = z2;
      }
      else {
        index = numAddedClipVertices * 3;
        ++numAddedClipVertices;
        t                         = dot1 / (dot1 - dot2);
        _clipVertices1[index]     = x1 + (x2 - x1) * t;
        _clipVertices1[index + 1] = y1 + (y2 - y1) * t;
        _clipVertices1[index + 2] = z1 + (z2 - z1) * t;
      }
    }
    else {
      if (dot2 > 0.f) {
        index = numAddedClipVertices * 3;
        ++numAddedClipVertices;
        t                         = dot1 / (dot1 - dot2);
        _clipVertices1[index]     = x1 + (x2 - x1) * t;
        _clipVertices1[index + 1] = y1 + (y2 - y1) * t;
        _clipVertices1[index + 2] = z1 + (z2 - z1) * t;
        index                     = numAddedClipVertices * 3;
        ++numAddedClipVertices;
        _clipVertices1[index]     = x2;
        _clipVertices1[index + 1] = y2;
        _clipVertices1[index + 2] = z2;
      }
    }
    x1   = x2;
    y1   = y2;
    z1   = z2;
    dot1 = dot2;
  }

  numClipVertices = numAddedClipVertices;
  if (numClipVertices == 0) {
    return;
  }
  numAddedClipVertices = 0;
  index                = (numClipVertices - 1) * 3;
  x1                   = _clipVertices1[index];
  y1                   = _clipVertices1[index + 1];
  z1                   = _clipVertices1[index + 2];
  dot1
    = (x1 - cx + s1x) * -n1x + (y1 - cy + s1y) * -n1y + (z1 - cz + s1z) * -n1z;

  for (unsigned int i = 0; i < numClipVertices; ++i) {
    index = i * 3;
    x2    = _clipVertices1[index];
    y2    = _clipVertices1[index + 1];
    z2    = _clipVertices1[index + 2];
    dot2  = (x2 - cx + s1x) * -n1x + (y2 - cy + s1y) * -n1y
           + (z2 - cz + s1z) * -n1z;
    if (dot1 > 0.f) {
      if (dot2 > 0.f) {
        index = numAddedClipVertices * 3;
        ++numAddedClipVertices;
        _clipVertices2[index]     = x2;
        _clipVertices2[index + 1] = y2;
        _clipVertices2[index + 2] = z2;
      }
      else {
        index = numAddedClipVertices * 3;
        ++numAddedClipVertices;
        t                         = dot1 / (dot1 - dot2);
        _clipVertices2[index]     = x1 + (x2 - x1) * t;
        _clipVertices2[index + 1] = y1 + (y2 - y1) * t;
        _clipVertices2[index + 2] = z1 + (z2 - z1) * t;
      }
    }
    else {
      if (dot2 > 0.f) {
        index = numAddedClipVertices * 3;
        ++numAddedClipVertices;
        t                         = dot1 / (dot1 - dot2);
        _clipVertices2[index]     = x1 + (x2 - x1) * t;
        _clipVertices2[index + 1] = y1 + (y2 - y1) * t;
        _clipVertices2[index + 2] = z1 + (z2 - z1) * t;
        index                     = numAddedClipVertices * 3;
        ++numAddedClipVertices;
        _clipVertices2[index]     = x2;
        _clipVertices2[index + 1] = y2;
        _clipVertices2[index + 2] = z2;
      }
    }
    x1   = x2;
    y1   = y2;
    z1   = z2;
    dot1 = dot2;
  }

  numClipVertices = numAddedClipVertices;
  if (numClipVertices == 0) {
    return;
  }
  numAddedClipVertices = 0;
  index                = (numClipVertices - 1) * 3;
  x1                   = _clipVertices2[index];
  y1                   = _clipVertices2[index + 1];
  z1                   = _clipVertices2[index + 2];
  dot1
    = (x1 - cx + s2x) * -n2x + (y1 - cy + s2y) * -n2y + (z1 - cz + s2z) * -n2z;

  for (unsigned int i = 0; i < numClipVertices; ++i) {
    index = i * 3;
    x2    = _clipVertices2[index];
    y2    = _clipVertices2[index + 1];
    z2    = _clipVertices2[index + 2];
    dot2  = (x2 - cx + s2x) * -n2x + (y2 - cy + s2y) * -n2y
           + (z2 - cz + s2z) * -n2z;
    if (dot1 > 0.f) {
      if (dot2 > 0.f) {
        index = numAddedClipVertices * 3;
        ++numAddedClipVertices;
        _clipVertices1[index]     = x2;
        _clipVertices1[index + 1] = y2;
        _clipVertices1[index + 2] = z2;
      }
      else {
        index = numAddedClipVertices * 3;
        ++numAddedClipVertices;
        t                         = dot1 / (dot1 - dot2);
        _clipVertices1[index]     = x1 + (x2 - x1) * t;
        _clipVertices1[index + 1] = y1 + (y2 - y1) * t;
        _clipVertices1[index + 2] = z1 + (z2 - z1) * t;
      }
    }
    else {
      if (dot2 > 0.f) {
        index = numAddedClipVertices * 3;
        ++numAddedClipVertices;
        t                         = dot1 / (dot1 - dot2);
        _clipVertices1[index]     = x1 + (x2 - x1) * t;
        _clipVertices1[index + 1] = y1 + (y2 - y1) * t;
        _clipVertices1[index + 2] = z1 + (z2 - z1) * t;
        index                     = numAddedClipVertices * 3;
        ++numAddedClipVertices;
        _clipVertices1[index]     = x2;
        _clipVertices1[index + 1] = y2;
        _clipVertices1[index + 2] = z2;
      }
    }
    x1   = x2;
    y1   = y2;
    z1   = z2;
    dot1 = dot2;
  }

  numClipVertices = numAddedClipVertices;
  if (swap) {
    BoxShape* tb = b1;
    b1           = b2;
    b2           = tb;
  }
  if (numClipVertices == 0) {
    return;
  }
  bool flipped = (b1 != shape1);
  if (numClipVertices > 4) {
    x1                  = (q1x + q2x + q3x + q4x) * 0.25f;
    y1                  = (q1y + q2y + q3y + q4y) * 0.25f;
    z1                  = (q1z + q2z + q3z + q4z) * 0.25f;
    n1x                 = q1x - x1;
    n1y                 = q1y - y1;
    n1z                 = q1z - z1;
    n2x                 = q2x - x1;
    n2y                 = q2y - y1;
    n2z                 = q2z - z1;
    unsigned int index1 = 0;
    unsigned int index2 = 0;
    unsigned int index3 = 0;
    unsigned int index4 = 0;
    float maxDot        = -_inf;
    minDot              = _inf;

    // i = numClipVertices;
    // while(i--){
    for (unsigned int i = 0; i < numClipVertices; ++i) {
      _used[i] = false;
      index    = i * 3;
      x1       = _clipVertices1[index];
      y1       = _clipVertices1[index + 1];
      z1       = _clipVertices1[index + 2];
      dot      = x1 * n1x + y1 * n1y + z1 * n1z;
      if (dot < minDot) {
        minDot = dot;
        index1 = i;
      }
      if (dot > maxDot) {
        maxDot = dot;
        index3 = i;
      }
    }

    _used[index1] = true;
    _used[index3] = true;
    maxDot        = -_inf;
    minDot        = _inf;

    for (unsigned int i = 0; i < numClipVertices; ++i) {
      if (_used[i]) {
        continue;
      }
      index = i * 3;
      x1    = _clipVertices1[index];
      y1    = _clipVertices1[index + 1];
      z1    = _clipVertices1[index + 2];
      dot   = x1 * n2x + y1 * n2y + z1 * n2z;
      if (dot < minDot) {
        minDot = dot;
        index2 = i;
      }
      if (dot > maxDot) {
        maxDot = dot;
        index4 = i;
      }
    }

    index = index1 * 3;
    x1    = _clipVertices1[index];
    y1    = _clipVertices1[index + 1];
    z1    = _clipVertices1[index + 2];
    dot   = (x1 - cx) * nx + (y1 - cy) * ny + (z1 - cz) * nz;
    if (dot < 0.f) {
      manifold->addPoint(x1, y1, z1, nx, ny, nz, dot, flipped);
    }

    index = index2 * 3;
    x1    = _clipVertices1[index];
    y1    = _clipVertices1[index + 1];
    z1    = _clipVertices1[index + 2];
    dot   = (x1 - cx) * nx + (y1 - cy) * ny + (z1 - cz) * nz;
    if (dot < 0.f) {
      manifold->addPoint(x1, y1, z1, nx, ny, nz, dot, flipped);
    }

    index = index3 * 3;
    x1    = _clipVertices1[index];
    y1    = _clipVertices1[index + 1];
    z1    = _clipVertices1[index + 2];
    dot   = (x1 - cx) * nx + (y1 - cy) * ny + (z1 - cz) * nz;
    if (dot < 0.f) {
      manifold->addPoint(x1, y1, z1, nx, ny, nz, dot, flipped);
    }

    index = index4 * 3;
    x1    = _clipVertices1[index];
    y1    = _clipVertices1[index + 1];
    z1    = _clipVertices1[index + 2];
    dot   = (x1 - cx) * nx + (y1 - cy) * ny + (z1 - cz) * nz;
    if (dot < 0.f) {
      manifold->addPoint(x1, y1, z1, nx, ny, nz, dot, flipped);
    }
  }
  else {
    for (unsigned int i = 0; i < numClipVertices; ++i) {
      index = i * 3;
      x1    = _clipVertices1[index];
      y1    = _clipVertices1[index + 1];
      z1    = _clipVertices1[index + 2];
      dot   = (x1 - cx) * nx + (y1 - cy) * ny + (z1 - cz) * nz;
      if (dot < 0.f) {
        manifold->addPoint(x1, y1, z1, nx, ny, nz, dot, flipped);
      }
    }
  }
}

} // end of namespace OIMO
