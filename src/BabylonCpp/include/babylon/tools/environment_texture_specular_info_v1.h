#ifndef BABYLON_TOOLS_ENVIRONMENT_TEXTURE_SPECULAR_INFO_V1
#define BABYLON_TOOLS_ENVIRONMENT_TEXTURE_SPECULAR_INFO_V1

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/tools/buffer_image_data.h>

namespace BABYLON {

/**
 * @brief Defines the specular data enclosed in the file.
 * This corresponds to the version 1 of the data.
 */
class BABYLON_SHARED_EXPORT EnvironmentTextureSpecularInfoV1 {

public:
  EnvironmentTextureSpecularInfoV1();
  EnvironmentTextureSpecularInfoV1(
    const EnvironmentTextureSpecularInfoV1& other);
  EnvironmentTextureSpecularInfoV1(EnvironmentTextureSpecularInfoV1&& other);
  EnvironmentTextureSpecularInfoV1&
  operator=(const EnvironmentTextureSpecularInfoV1& other);
  EnvironmentTextureSpecularInfoV1&
  operator=(EnvironmentTextureSpecularInfoV1&& other);
  ~EnvironmentTextureSpecularInfoV1();

public:
  /**
   * Defines where the specular Payload is located. It is a runtime value only
   * not stored in the file.
   */
  std::optional<size_t> specularDataPosition;
  /**
   * This contains all the images data needed to reconstruct the cubemap.
   */
  std::vector<BufferImageData> mipmaps;
  /**
   * Defines the scale applied to environment texture. This manages the range of
   * LOD level used for IBL according to the roughness.
   */
  std::optional<float> lodGenerationScale;

}; // end of class EnvironmentTextureSpecularInfoV1

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_ENVIRONMENT_TEXTURE_SPECULAR_INFO_V1
