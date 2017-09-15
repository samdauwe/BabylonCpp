#include <babylon/math/position_normal_vertex.h>

namespace BABYLON {

PositionNormalVertex::PositionNormalVertex()
    : position{Vector3::Zero()}, normal{Vector3::Up()}
{
}

PositionNormalVertex::PositionNormalVertex(const Vector3& positionVal,
                                           const Vector3& normalVal)
    : position{positionVal}, normal{normalVal}
{
}

PositionNormalVertex::PositionNormalVertex(const PositionNormalVertex& other)
    : position{other.position}, normal{other.normal}
{
}

PositionNormalVertex::PositionNormalVertex(PositionNormalVertex&& other)
    : position{::std::move(other.position)}, normal{::std::move(other.normal)}
{
}

PositionNormalVertex& PositionNormalVertex::
operator=(const PositionNormalVertex& other)
{
  if (&other != this) {
    position = other.position;
    normal   = other.normal;
  }

  return *this;
}

PositionNormalVertex& PositionNormalVertex::
operator=(PositionNormalVertex&& other)
{
  if (&other != this) {
    position = ::std::move(other.position);
    normal   = ::std::move(other.normal);
  }

  return *this;
}

PositionNormalVertex::~PositionNormalVertex()
{
}

PositionNormalVertex PositionNormalVertex::copy() const
{
  return PositionNormalVertex(*this);
}

unique_ptr_t<PositionNormalVertex> PositionNormalVertex::clone() const
{
  return ::std::make_unique<PositionNormalVertex>(*this);
}

std::ostream& operator<<(std::ostream& os,
                         const PositionNormalVertex& positionNormalVertex)
{
  os << "{\"Position\":" << positionNormalVertex.position
     << ",\"Normal\":" << positionNormalVertex.normal << "}";
  return os;
}

} // end of namespace BABYLON
