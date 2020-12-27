#ifndef BABYLON_MISC_ENVIRONMENT_TEXTURE_INFO
#define BABYLON_MISC_ENVIRONMENT_TEXTURE_INFO

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>
#include <babylon/misc/environment_texture_irradiance_info_v1.h>
#include <babylon/misc/environment_texture_specular_info_v1.h>

namespace BABYLON {

FWD_CLASS_SPTR(EnvironmentTextureInfo)

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
  ~EnvironmentTextureInfo(); // = default

  /**
   * @brief Creates raw texture data from parsed data.
   * @param parsedEnvironmentTextureInfo defines EnvironmentTextureInfo data
   * @returns the parsed raw texture data
   */
  static EnvironmentTextureInfoPtr Parse(const json& parsedManifest);

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
  EnvironmentTextureIrradianceInfoV1Ptr irradiance;

  /**
   * Specular information stored in the file.
   */
  EnvironmentTextureSpecularInfoV1Ptr specular;

}; // end of class EnvironmentTextureInfo

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_ENVIRONMENT_TEXTURE_INFO
