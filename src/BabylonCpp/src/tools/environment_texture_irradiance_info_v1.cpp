#include <babylon/tools/environment_texture_irradiance_info_v1.h>

#include <babylon/core/json_util.h>

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

EnvironmentTextureIrradianceInfoV1Ptr
EnvironmentTextureIrradianceInfoV1::Parse(const json& parsedIrradiance)
{
  EnvironmentTextureIrradianceInfoV1 irradiance;
  irradiance.x = json_util::get_array<float>(parsedIrradiance, "x");
  irradiance.y = json_util::get_array<float>(parsedIrradiance, "y");
  irradiance.z = json_util::get_array<float>(parsedIrradiance, "z");

  irradiance.xx = json_util::get_array<float>(parsedIrradiance, "xx");
  irradiance.yy = json_util::get_array<float>(parsedIrradiance, "yy");
  irradiance.zz = json_util::get_array<float>(parsedIrradiance, "zz");

  irradiance.yz = json_util::get_array<float>(parsedIrradiance, "yz");
  irradiance.zx = json_util::get_array<float>(parsedIrradiance, "zx");
  irradiance.xy = json_util::get_array<float>(parsedIrradiance, "xy");

  return std::make_shared<EnvironmentTextureIrradianceInfoV1>(irradiance);
}

} // end of namespace BABYLON
