#include <babylon/meshes/csg/polygon.h>

#include <algorithm>
#include <sstream>

#include <babylon/meshes/csg/vertex.h>

namespace BABYLON {

CSG::Polygon::Polygon()
    : shared{PolygonOptions{0, 0, 0}}, plane{std::make_pair(false, Plane())}
{
}

CSG::Polygon::Polygon(const std::vector<Vertex>& iVertices,
                      const PolygonOptions& iShared)
    : vertices{iVertices}, shared{iShared}
{
  plane = Plane::FromPoints(vertices[0].pos, vertices[1].pos, vertices[2].pos);
}

CSG::Polygon::Polygon(const BABYLON::CSG::Polygon& otherPolygon) = default;

CSG::Polygon::Polygon(BABYLON::CSG::Polygon&& otherPolygon) = default;

CSG::Polygon& CSG::Polygon::operator=(const BABYLON::CSG::Polygon& otherPolygon) = default;

CSG::Polygon& CSG::Polygon::operator=(BABYLON::CSG::Polygon&& otherPolygon) = default;

CSG::Polygon::~Polygon() = default;

CSG::Polygon CSG::Polygon::clone() const
{
  return Polygon(*this);
}

std::unique_ptr<CSG::Polygon> CSG::Polygon::cloneToNewObject() const
{
  return std::make_unique<Polygon>(*this);
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
    vertex.flip();
  }
  if (plane.first) {
    auto planeTmp = plane.second;
    planeTmp.flip();
    plane.second = planeTmp;
  }
}

} // end of namespace BABYLON
