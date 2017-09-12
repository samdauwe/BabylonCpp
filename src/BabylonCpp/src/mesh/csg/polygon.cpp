#include <babylon/mesh/csg/polygon.h>

#include <babylon/mesh/csg/vertex.h>

namespace BABYLON {

CSG::Polygon::Polygon(const std::vector<Vertex*>& _vertices,
                      const PolygonOptions& _shared)
    : vertices{_vertices}, shared{_shared}, _hasPlane{true}
{
  plane = Plane::FromPoints(vertices[0]->pos, vertices[1]->pos,
                            vertices[2]->pos, _hasPlane);
}

CSG::Polygon::Polygon(const BABYLON::CSG::Polygon& otherPolygon)
    : vertices{otherPolygon.vertices}
    , shared{otherPolygon.shared}
    , plane{otherPolygon.plane}
{
}

CSG::Polygon::Polygon(BABYLON::CSG::Polygon&& otherPolygon)
{
  *this = ::std::move(otherPolygon);
}

CSG::Polygon& CSG::Polygon::operator=(const BABYLON::CSG::Polygon& otherPolygon)
{
  if (&otherPolygon != this) {
    vertices = otherPolygon.vertices;
    shared   = otherPolygon.shared;
    plane    = otherPolygon.plane;
  }

  return *this;
}

CSG::Polygon& CSG::Polygon::operator=(BABYLON::CSG::Polygon&& otherPolygon)
{
  if (&otherPolygon != this) {
    vertices = ::std::move(otherPolygon.vertices);
    shared   = ::std::move(otherPolygon.shared);
    plane    = ::std::move(otherPolygon.plane);
  }

  return *this;
}

CSG::Polygon::~Polygon()
{
}

CSG::Polygon CSG::Polygon::clone() const
{
  return Polygon(*this);
}

CSG::Polygon* CSG::Polygon::cloneToNewObject() const
{
  return new Polygon(*this);
}

namespace CSG {
std::ostream& operator<<(std::ostream& os, const Polygon& polygon)
{
  os << "{\"Vertices\":[";
  if (polygon.vertices.size() > 0) {
    for (unsigned int i = 0; i < polygon.vertices.size() - 1; ++i) {
      os << polygon.vertices[i] << ",";
    }
    os << polygon.vertices.back();
  }
  // os << "],\"Shared\":" << polygon.shared << ",\"Plane\":" << polygon.plane
  //   << "}";
  return os;
}
} // namespace CSG

std::string CSG::Polygon::toString() const
{
  std::ostringstream oss;
  oss << (*this);
  return oss.str();
}

void CSG::Polygon::flip()
{
  std::reverse(vertices.begin(), vertices.end());
  for (auto& vertex : vertices) {
    vertex->flip();
  }
  plane.flip();
}

bool CSG::Polygon::hasPlane() const
{
  return _hasPlane;
}

} // end of namespace BABYLON
