#include <babylon/math/position_normal_texture_vertex.h>

namespace BABYLON {

PositionNormalTextureVertex::PositionNormalTextureVertex(
  const Vector3& iposition, const Vector3& inormal, const Vector2& iuv)
    : position{iposition}, normal{inormal}, uv{iuv}
{
}

PositionNormalTextureVertex::PositionNormalTextureVertex(
  const PositionNormalTextureVertex& other) = default;

PositionNormalTextureVertex::PositionNormalTextureVertex(
  PositionNormalTextureVertex&& other) = default;

PositionNormalTextureVertex& PositionNormalTextureVertex::
operator=(const PositionNormalTextureVertex& other) = default;

PositionNormalTextureVertex& PositionNormalTextureVertex::
operator=(PositionNormalTextureVertex&& other) = default;

PositionNormalTextureVertex::~PositionNormalTextureVertex() = default;

PositionNormalTextureVertex PositionNormalTextureVertex::copy() const
{
  return PositionNormalTextureVertex(*this);
}

std::unique_ptr<PositionNormalTextureVertex>
PositionNormalTextureVertex::clone() const
{
  return std::make_unique<PositionNormalTextureVertex>(*this);
}

std::ostream&
operator<<(std::ostream& os,
           const PositionNormalTextureVertex& positionNormalTextureVertex)
{
  os << "{\"Position\":" << positionNormalTextureVertex.position
     << ",\"Normal\":" << positionNormalTextureVertex.normal
     << ",\"UV\":" << positionNormalTextureVertex.uv << "}";
  return os;
}

} // end of namespace BABYLON
