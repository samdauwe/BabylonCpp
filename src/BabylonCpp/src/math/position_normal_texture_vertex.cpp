#include <babylon/math/position_normal_texture_vertex.h>

namespace BABYLON {

PositionNormalTextureVertex::PositionNormalTextureVertex(
  const Vector3& iposition, const Vector3& inormal, const Vector2& iuv)
    : position{iposition}, normal{inormal}, uv{iuv}
{
}

PositionNormalTextureVertex::PositionNormalTextureVertex(
  const PositionNormalTextureVertex& other)
    : position{other.position}, normal{other.normal}, uv{other.uv}
{
}

PositionNormalTextureVertex::PositionNormalTextureVertex(
  PositionNormalTextureVertex&& other)
    : position{std::move(other.position)}
    , normal{std::move(other.normal)}
    , uv{std::move(other.uv)}
{
}

PositionNormalTextureVertex& PositionNormalTextureVertex::
operator=(const PositionNormalTextureVertex& other)
{
  if (&other != this) {
    position = other.position;
    normal   = other.normal;
    uv       = other.uv;
  }

  return *this;
}

PositionNormalTextureVertex& PositionNormalTextureVertex::
operator=(PositionNormalTextureVertex&& other)
{
  if (&other != this) {
    position = std::move(other.position);
    normal   = std::move(other.normal);
    uv       = std::move(other.uv);
  }

  return *this;
}

PositionNormalTextureVertex::~PositionNormalTextureVertex()
{
}

PositionNormalTextureVertex PositionNormalTextureVertex::copy() const
{
  return PositionNormalTextureVertex(*this);
}

std::unique_ptr<PositionNormalTextureVertex>
PositionNormalTextureVertex::clone() const
{
  return std_util::make_unique<PositionNormalTextureVertex>(*this);
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
