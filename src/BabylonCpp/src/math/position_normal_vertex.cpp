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

PositionNormalVertex::PositionNormalVertex(const PositionNormalVertex& other) = default;
PositionNormalVertex::PositionNormalVertex(PositionNormalVertex&& other) = default;
PositionNormalVertex& PositionNormalVertex::operator=(const PositionNormalVertex& other) = default;
PositionNormalVertex& PositionNormalVertex::operator=(PositionNormalVertex&& other) = default;
PositionNormalVertex::~PositionNormalVertex() = default;

PositionNormalVertex PositionNormalVertex::copy() const
{
  return PositionNormalVertex(*this);
}

std::unique_ptr<PositionNormalVertex> PositionNormalVertex::clone() const
{
  return std::make_unique<PositionNormalVertex>(*this);
}

std::ostream& operator<<(std::ostream& os,
                         const PositionNormalVertex& positionNormalVertex)
{
  os << "{\"Position\":" << positionNormalVertex.position
     << ",\"Normal\":" << positionNormalVertex.normal << "}";
  return os;
}

} // end of namespace BABYLON
