#ifndef BABYLON_MISC_ENVIRONMENT_TEXTURE_IRRADIANCE_INFO_V1
#define BABYLON_MISC_ENVIRONMENT_TEXTURE_IRRADIANCE_INFO_V1

#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

using json = nlohmann::json;

namespace BABYLON {

class EnvironmentTextureIrradianceInfoV1;
using EnvironmentTextureIrradianceInfoV1Ptr
  = std::shared_ptr<EnvironmentTextureIrradianceInfoV1>;

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
  ~EnvironmentTextureIrradianceInfoV1(); // = default

  /**
   * @brief Creates the irradiance information from the parsed irradiance data.
   * @param parsedIrradiance defines irradiance data
   * @returns the parsed irradiance information
   */
  static EnvironmentTextureIrradianceInfoV1Ptr
  Parse(const json& parsedIrradiance);

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

#endif // end of BABYLON_MISC_ENVIRONMENT_TEXTURE_IRRADIANCE_INFO_V1
