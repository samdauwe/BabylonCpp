#include <babylon/tools/environment_texture_specular_info_v1.h>

namespace BABYLON {

EnvironmentTextureSpecularInfoV1::EnvironmentTextureSpecularInfoV1()
    : specularDataPosition{std::nullopt}, lodGenerationScale{0.f}
{
}

EnvironmentTextureSpecularInfoV1::EnvironmentTextureSpecularInfoV1(
  const EnvironmentTextureSpecularInfoV1& other)
    : specularDataPosition{other.specularDataPosition}
    , mipmaps{other.mipmaps}
    , lodGenerationScale{other.lodGenerationScale}
{
}

EnvironmentTextureSpecularInfoV1::EnvironmentTextureSpecularInfoV1(
  EnvironmentTextureSpecularInfoV1&& other)
    : specularDataPosition{std::move(other.specularDataPosition)}
    , mipmaps{std::move(other.mipmaps)}
    , lodGenerationScale{std::move(other.lodGenerationScale)}
{
}

EnvironmentTextureSpecularInfoV1& EnvironmentTextureSpecularInfoV1::
operator=(const EnvironmentTextureSpecularInfoV1& other)
{
  if (&other != this) {
    specularDataPosition = other.specularDataPosition;
    mipmaps              = other.mipmaps;
    lodGenerationScale   = other.lodGenerationScale;
  }

  return *this;
}

EnvironmentTextureSpecularInfoV1& EnvironmentTextureSpecularInfoV1::
operator=(EnvironmentTextureSpecularInfoV1&& other)
{
  if (&other != this) {
    specularDataPosition = std::move(other.specularDataPosition);
    mipmaps              = std::move(other.mipmaps);
    lodGenerationScale   = std::move(other.lodGenerationScale);
  }

  return *this;
}

EnvironmentTextureSpecularInfoV1::~EnvironmentTextureSpecularInfoV1()
{
}

} // end of namespace BABYLON
