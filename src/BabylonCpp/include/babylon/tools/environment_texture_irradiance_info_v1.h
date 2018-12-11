#ifndef BABYLON_TOOLS_ENVIRONMENT_TEXTURE_IRRADIANCE_INFO_V1
#define BABYLON_TOOLS_ENVIRONMENT_TEXTURE_IRRADIANCE_INFO_V1

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

/**
 * @brief Defines the required storage to save the environment irradiance
 * information.
 */
class BABYLON_SHARED_EXPORT EnvironmentTextureIrradianceInfoV1 {

public:
  EnvironmentTextureIrradianceInfoV1();
  EnvironmentTextureIrradianceInfoV1(
    const EnvironmentTextureIrradianceInfoV1& other);
  EnvironmentTextureIrradianceInfoV1(
    EnvironmentTextureIrradianceInfoV1&& other);
  EnvironmentTextureIrradianceInfoV1&
  operator=(const EnvironmentTextureIrradianceInfoV1& other);
  EnvironmentTextureIrradianceInfoV1&
  operator=(EnvironmentTextureIrradianceInfoV1&& other);
  ~EnvironmentTextureIrradianceInfoV1();

public:
  Float32Array x;
  Float32Array y;
  Float32Array z;

  Float32Array xx;
  Float32Array yy;
  Float32Array zz;

  Float32Array yz;
  Float32Array zx;
  Float32Array xy;

}; // end of class EnvironmentTextureIrradianceInfoV1

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_ENVIRONMENT_TEXTURE_IRRADIANCE_INFO_V1
