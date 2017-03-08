#include <oimo/collision/narrowphase/tetra_tetra_collision_detector.h>

#include <oimo/collision/shape/tetra_shape.h>
#include <oimo/constraint/contact/contact_manifold.h>
#include <oimo/math/vec3.h>

namespace OIMO {

TetraTetraCollisionDetector::TetraTetraCollisionDetector() : CollisionDetector{}
{
}

TetraTetraCollisionDetector::~TetraTetraCollisionDetector()
{
}

void TetraTetraCollisionDetector::detectCollision(Shape* shape1, Shape* shape2,
                                                  ContactManifold* manifold)
{
  /**
   * Each tetra is represented by four 3D triangles. The only quick way of
   * finding if another tetra is within the other tetra is to see if a single
   * vertex is within the triangles of the other tetra. So, for example, a tetra
   * is represented by points B, C, D and E with triangles BCD, BCE, DCE and
   * BDE. There is another tetra with a vertex A which was detected for
   * collision by the broadphase. Now, if the point A is between ALL triangles
   * of the other tetra (BCD, BCE, etc.) then the collision is valid and we can
   * pass point A to the manifold. Since the only points on the tetra are the 4
   * vertices, collision detection is not so complex. However, it can be
   * time-consuming because we need to split the 3D triangles into three 2D
   * triangles each for collision detection.
   */
  if (shape1->type != Shape::Type::SHAPE_TETRA
      || shape2->type != Shape::Type::SHAPE_TETRA) {
    return;
  }
  auto tet1 = dynamic_cast<TetraShape*>(shape1);
  auto tet2 = dynamic_cast<TetraShape*>(shape2);
  // Faces
  auto& fs1 = tet1->faces;
  // Vertices
  auto& vs1 = tet1->verts;
  auto& vs2 = tet2->verts;
  // Triangle vertices `j1`, `j2` and `j3`
  Vertex j1, j2, j3;
  unsigned int ts = 0;
  for (unsigned int i = 0; i < 4; ++i) {
    const auto& vec = vs1[i];
    for (unsigned int j = 0; j < 4; ++j) {
      j1 = vs2[fs1[i].a]; // a
      j2 = vs2[fs1[i].b]; // b
      j3 = vs2[fs1[i].c]; // c

      if (_tricheck(_pt(vec.x, vec.y), _pt(j1.x, j1.y), _pt(j2.x, j2.y),
                    _pt(j3.x, j3.y))
          && _tricheck(_pt(vec.x, vec.z), _pt(j1.x, j1.z), _pt(j2.x, j2.z),
                       _pt(j3.x, j3.z))
          && _tricheck(_pt(vec.z, vec.y), _pt(j1.z, j1.y), _pt(j2.z, j2.y),
                       _pt(j3.z, j3.y))) {
        ++ts;
      }

      if (ts == 4) {
        // Only add point if it is inside all 4 triangles
        manifold->addPoint(vec.x, vec.y, vec.z);
      }
    }
  }
}

bool TetraTetraCollisionDetector::_tricheck(const Vec3& p, const Vec3& p0,
                                            const Vec3& p1,
                                            const Vec3& p2) const
{
  float A = 0.5f * (-p1.y * p2.x + p0.y * (-p1.x + p2.x) + p0.x * (p1.y - p2.y)
                    + p1.x * p2.y);
  float sg = A < 0.f ? -1.f : 1.f;
  float s
    = (p0.y * p2.x - p0.x * p2.y + (p2.y - p0.y) * p.x + (p0.x - p2.x) * p.y)
      * sg;
  float t
    = (p0.x * p1.y - p0.y * p1.x + (p0.y - p1.y) * p.x + (p1.x - p0.x) * p.y)
      * sg;
  return s > 0.f && t > 0.f && (s + t) < 2.f * A * sg;
}

Vec3 TetraTetraCollisionDetector::_pt(float x, float y) const
{
  return Vec3(x, y, 0.f);
}

} // end of namespace OIMO
