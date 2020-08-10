#include <babylon/meshes/csg/vertex.h>

#include <sstream>

namespace BABYLON {

CSG::Vertex::Vertex(const Vector3& iPos, const Vector3& iNormal, const std::optional<Vector2>& iUv,
                    const std::optional<Color4>& iVertColor)
    : pos{iPos}, normal{iNormal}, uv{iUv}, vertColor{iVertColor}
{
}

CSG::Vertex::Vertex(const BABYLON::CSG::Vertex& otherVertex) = default;
CSG::Vertex::Vertex(BABYLON::CSG::Vertex&& otherVertex)      = default;
CSG::Vertex& CSG::Vertex::operator=(const BABYLON::CSG::Vertex& otherVertex) = default;
CSG::Vertex& CSG::Vertex::operator=(BABYLON::CSG::Vertex&& otherVertex) = default;
CSG::Vertex::~Vertex()                                                  = default;

CSG::Vertex CSG::Vertex::clone() const
{
  return Vertex(*this);
}

std::unique_ptr<CSG::Vertex> CSG::Vertex::cloneToNewObject() const
{
  return std::make_unique<Vertex>(*this);
}

namespace CSG {
std::ostream& operator<<(std::ostream& os, const BABYLON::CSG::Vertex& vertex)
{
  os << "{\"Position\":" << vertex.pos << ",\"Normal\":" << vertex.normal << ",\"UV\":";
  if (vertex.uv) {
    os << *vertex.uv;
  }
  else {
    os << "\"undefined\"";
  }
  os << ",\"Color\":";
  if (vertex.vertColor) {
    os << *vertex.vertColor;
  }
  else {
    os << "\"undefined\"";
  }
  os << "}";
  return os;
}
} // namespace CSG

std::string CSG::Vertex::toString() const
{
  std::ostringstream oss;
  oss << (*this);
  return oss.str();
}

void CSG::Vertex::flip()
{
  normal = normal.scale(-1.f);
}

CSG::Vertex CSG::Vertex::interpolate(const BABYLON::CSG::Vertex& other, float t)
{
  return Vertex(Vector3::Lerp(pos, other.pos, t), Vector3::Lerp(normal, other.normal, t),
                uv && other.uv ? std::optional<Vector2>(Vector2::Lerp(*uv, *other.uv, t)) :
                                 std::nullopt,
                vertColor && other.vertColor ?
                  std::optional<Color4>(Color4::Lerp(*vertColor, *other.vertColor, t)) :
                  std::nullopt);
}

} // end of namespace BABYLON
