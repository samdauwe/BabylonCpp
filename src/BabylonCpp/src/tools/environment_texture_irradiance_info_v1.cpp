#include <babylon/tools/environment_texture_irradiance_info_v1.h>

namespace BABYLON {

EnvironmentTextureIrradianceInfoV1::EnvironmentTextureIrradianceInfoV1()
{
}

EnvironmentTextureIrradianceInfoV1::EnvironmentTextureIrradianceInfoV1(
  const EnvironmentTextureIrradianceInfoV1& other)
    : x{other.x}
    , y{other.y}
    , z{other.z}
    , xx{other.xx}
    , yy{other.yy}
    , zz{other.zz}
    , yz{other.yz}
    , zx{other.zx}
    , xy{other.xy}
{
}

EnvironmentTextureIrradianceInfoV1::EnvironmentTextureIrradianceInfoV1(
  EnvironmentTextureIrradianceInfoV1&& other)
    : x{std::move(other.x)}
    , y{std::move(other.y)}
    , z{std::move(other.z)}
    , xx{std::move(other.xx)}
    , yy{std::move(other.yy)}
    , zz{std::move(other.zz)}
    , yz{std::move(other.yz)}
    , zx{std::move(other.zx)}
    , xy{std::move(other.xy)}
{
}

EnvironmentTextureIrradianceInfoV1& EnvironmentTextureIrradianceInfoV1::
operator=(const EnvironmentTextureIrradianceInfoV1& other)
{
  if (&other != this) {
    x  = other.x;
    y  = other.y;
    z  = other.z;
    xx = other.xx;
    yy = other.yy;
    zz = other.zz;
    yz = other.yz;
    zx = other.zx;
    xy = other.xy;
  }

  return *this;
}

EnvironmentTextureIrradianceInfoV1& EnvironmentTextureIrradianceInfoV1::
operator=(EnvironmentTextureIrradianceInfoV1&& other)
{
  if (&other != this) {
    x  = std::move(other.x);
    y  = std::move(other.y);
    z  = std::move(other.z);
    xx = std::move(other.xx);
    yy = std::move(other.yy);
    zz = std::move(other.zz);
    yz = std::move(other.yz);
    zx = std::move(other.zx);
    xy = std::move(other.xy);
  }

  return *this;
}

EnvironmentTextureIrradianceInfoV1::~EnvironmentTextureIrradianceInfoV1()
{
}

} // end of namespace BABYLON
