#include <babylon/tools/environment_texture_info.h>

namespace BABYLON {

EnvironmentTextureInfo::EnvironmentTextureInfo()
    : version{0}, width{0}, irradiance{std::nullopt}, specular{std::nullopt}
{
}

EnvironmentTextureInfo::EnvironmentTextureInfo(
  const EnvironmentTextureInfo& other)
    : version{other.version}
    , width{other.width}
    , irradiance{other.irradiance}
    , specular{other.specular}
{
}

EnvironmentTextureInfo::EnvironmentTextureInfo(EnvironmentTextureInfo&& other)
    : version{std::move(other.version)}
    , width{std::move(other.width)}
    , irradiance{std::move(other.irradiance)}
    , specular{std::move(other.specular)}
{
}

EnvironmentTextureInfo& EnvironmentTextureInfo::
operator=(const EnvironmentTextureInfo& other)
{
  if (&other != this) {
    version    = other.version;
    width      = other.width;
    irradiance = other.irradiance;
    specular   = other.specular;
  }

  return *this;
}

EnvironmentTextureInfo& EnvironmentTextureInfo::
operator=(EnvironmentTextureInfo&& other)
{
  if (&other != this) {
    version    = std::move(other.version);
    width      = std::move(other.width);
    irradiance = std::move(other.irradiance);
    specular   = std::move(other.specular);
  }

  return *this;
}

EnvironmentTextureInfo::~EnvironmentTextureInfo()
{
}

} // end of namespace BABYLON
