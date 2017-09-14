#include <babylon/mesh/csg/vertex.h>

namespace BABYLON {

CSG::Vertex::Vertex(const Vector3& _pos, const Vector3& _normal,
                    const Vector2& _uv)
    : pos{_pos}, normal{_normal}, uv{_uv}
{
}

CSG::Vertex::Vertex(const BABYLON::CSG::Vertex& otherVertex)
    : pos{otherVertex.pos}, normal{otherVertex.normal}, uv{otherVertex.uv}
{
}

CSG::Vertex::Vertex(BABYLON::CSG::Vertex&& otherVertex)
{
  *this = ::std::move(otherVertex);
}

CSG::Vertex& CSG::Vertex::operator=(const BABYLON::CSG::Vertex& otherVertex)
{
  if (&otherVertex != this) {
    pos    = otherVertex.pos;
    normal = otherVertex.normal;
    uv     = otherVertex.uv;
  }

  return *this;
}

CSG::Vertex& CSG::Vertex::operator=(BABYLON::CSG::Vertex&& otherVertex)
{
  if (&otherVertex != this) {
    pos    = ::std::move(otherVertex.pos);
    normal = ::std::move(otherVertex.normal);
    uv     = ::std::move(otherVertex.uv);
  }

  return *this;
}

CSG::Vertex::~Vertex()
{
}

CSG::Vertex CSG::Vertex::clone() const
{
  return Vertex(*this);
}

CSG::Vertex* CSG::Vertex::cloneToNewObject() const
{
  return new Vertex(*this);
}

namespace CSG {
std::ostream& operator<<(std::ostream& os, const BABYLON::CSG::Vertex& vertex)
{
  os << "{\"Position\":" << vertex.pos << ",\"Normal\":" << vertex.normal
     << ",\"UV\":" << vertex.uv << "}";
  return os;
}
} // namespace CSG

string_t CSG::Vertex::toString() const
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
  return Vertex(Vector3::Lerp(pos, other.pos, t),
                Vector3::Lerp(normal, other.normal, t),
                Vector2::Lerp(uv, other.uv, t));
}

} // end of namespace BABYLON
