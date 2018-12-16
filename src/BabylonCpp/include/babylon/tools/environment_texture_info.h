#ifndef BABYLON_TOOLS_ENVIRONMENT_TEXTURE_INFO
#define BABYLON_TOOLS_ENVIRONMENT_TEXTURE_INFO

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/tools/environment_texture_irradiance_info_v1.h>
#include <babylon/tools/environment_texture_specular_info_v1.h>

namespace BABYLON {

/**
 * @brief Raw texture data and descriptor sufficient for WebGL texture upload.
 */
class BABYLON_SHARED_EXPORT EnvironmentTextureInfo {

public:
  EnvironmentTextureInfo();
  EnvironmentTextureInfo(const EnvironmentTextureInfo& other);
  EnvironmentTextureInfo(EnvironmentTextureInfo&& other);
  EnvironmentTextureInfo& operator=(const EnvironmentTextureInfo& other);
  EnvironmentTextureInfo& operator=(EnvironmentTextureInfo&& other);
  ~EnvironmentTextureInfo();

  /**
   * @brief Creates raw texture data from parsed data.
   * @param parsedEnvironmentTextureInfo defines EnvironmentTextureInfo data
   * @returns the parsed raw texture data
   */
  static EnvironmentTextureInfo Parse(const json& parsedEnvironmentTextureInfo);

public:
  /**
   * Version of the environment map
   */
  unsigned int version;

  /**
   * Width of image
   */
  int width;

  /**
   * Irradiance information stored in the file.
   */
  std::optional<EnvironmentTextureIrradianceInfoV1> irradiance;

  /**
   * Specular information stored in the file.
   */
  std::optional<EnvironmentTextureSpecularInfoV1> specular;

}; // end of class EnvironmentTextureInfo

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_ENVIRONMENT_TEXTURE_INFO
