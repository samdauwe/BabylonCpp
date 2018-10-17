#include <babylon/mesh/csg/plane.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/mesh/csg/polygon.h>
#include <babylon/mesh/csg/vertex.h>

namespace BABYLON {

const float CSG::Plane::EPSILON = 1e-5f;

CSG::Plane::Plane()
{
}

CSG::Plane::Plane(const Vector3& _normal, float _w) : normal{_normal}, w{_w}
{
}

BABYLON::CSG::Plane::Plane(const BABYLON::CSG::Plane& otherPlane)
    : normal{otherPlane.normal}, w{otherPlane.w}
{
}

BABYLON::CSG::Plane::Plane(BABYLON::CSG::Plane&& otherPlane)
    : normal{std::move(otherPlane.normal)}, w{std::move(otherPlane.w)}
{
}

BABYLON::CSG::Plane& BABYLON::CSG::Plane::
operator=(const BABYLON::CSG::Plane& otherPlane)
{
  if (&otherPlane != this) {
    normal = otherPlane.normal;
    w      = otherPlane.w;
  }

  return *this;
}

BABYLON::CSG::Plane& BABYLON::CSG::Plane::
operator=(BABYLON::CSG::Plane&& otherPlane)
{
  if (&otherPlane != this) {
    normal = std::move(otherPlane.normal);
    w      = std::move(otherPlane.w);
  }

  return *this;
}

CSG::Plane::~Plane()
{
}

CSG::Plane CSG::Plane::clone() const
{
  return Plane(*this);
}

std::unique_ptr<CSG::Plane> CSG::Plane::cloneToNewObject() const
{
  return std::make_unique<BABYLON::CSG::Plane>(*this);
}

namespace CSG {
std::ostream& operator<<(std::ostream& os, const BABYLON::CSG::Plane& plane)
{
  os << "{\"Normal\":" << plane.normal << ",\"W\":" << plane.w << "}";
  return os;
}
} // namespace CSG

std::string CSG::Plane::toString() const
{
  std::ostringstream oss;
  oss << (*this);
  return oss.str();
}

void CSG::Plane::flip()
{
  normal.scaleInPlace(-1.f);
  w = -w;
}

void BABYLON::CSG::Plane::splitPolygon(
  const BABYLON::CSG::Polygon& polygon,
  std::vector<BABYLON::CSG::Polygon>& coplanarFront,
  std::vector<BABYLON::CSG::Polygon>& coplanarBack,
  std::vector<BABYLON::CSG::Polygon>& front,
  std::vector<BABYLON::CSG::Polygon>& back)
{
  // Classify each point as well as the entire polygon into one of the above
  // four classes.
  int polygonType = 0;
  Int32Array types;
  for (unsigned int i = 0; i < polygon.vertices.size(); ++i) {
    const float t = Vector3::Dot(normal, polygon.vertices[i].pos) - w;
    int type
      = (t < -Plane::EPSILON) ? BACK : (t > Plane::EPSILON) ? FRONT : COPLANAR;
    polygonType |= type;
    types.emplace_back(type);
  }

  // std::cout << polygonType << std::endl;

  // Put the polygon in the correct list, splitting it when necessary.
  switch (polygonType) {
    case COPLANAR:
      (Vector3::Dot(normal, polygon.plane.second.normal) > 0 ? coplanarFront :
                                                               coplanarBack)
        .emplace_back(polygon);
      break;
    case FRONT:
      front.emplace_back(polygon);
      break;
    case BACK:
      back.emplace_back(polygon);
      break;
    case SPANNING:
      std::vector<BABYLON::CSG::Vertex> f, b;
      for (size_t i = 0; i < polygon.vertices.size(); ++i) {
        size_t j = (i + 1) % polygon.vertices.size();
        int ti = types[i], tj = types[j];
        Vertex vi = polygon.vertices[i], vj = polygon.vertices[j];
        if (ti != BACK) {
          f.emplace_back(vi);
        }
        if (ti != FRONT) {
          b.emplace_back(ti != BACK ? Vertex(vi) : vi);
        }
        if ((ti == SPANNING) || (tj == SPANNING)) {
          float t = (w - Vector3::Dot(normal, vi.pos))
                    / Vector3::Dot(normal, vj.pos.subtract(vi.pos));
          BABYLON::CSG::Vertex v = vi.interpolate(vj, t);
          f.emplace_back(BABYLON::CSG::Vertex(v));
          b.emplace_back(BABYLON::CSG::Vertex(v));
        }
      }
      if (f.size() >= 3) {
        Polygon poly(f, polygon.shared);

        if (poly.plane.first) {
          front.emplace_back(poly);
        }
      }

      if (b.size() >= 3) {
        Polygon poly(b, polygon.shared);

        if (poly.plane.first) {
          back.emplace_back(poly);
        }
      }

      break;
  }
}

std::pair<bool, CSG::Plane>
CSG::Plane::FromPoints(const Vector3& a, const Vector3& b, const Vector3& c)
{
  const auto v0 = c.subtract(a);
  const auto v1 = b.subtract(a);

  if (stl_util::almost_equal(v0.lengthSquared(), 0.f)
      || stl_util::almost_equal(v1.lengthSquared(), 0.f)) {
    return std::make_pair(false, Plane());
  }

  const auto n = Vector3::Normalize(Vector3::Cross(v0, v1));
  return std::make_pair(true, Plane(n, Vector3::Dot(n, a)));
}

} // end of namespace BABYLON
