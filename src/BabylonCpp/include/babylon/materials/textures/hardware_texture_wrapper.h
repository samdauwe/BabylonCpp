#ifndef BABYLON_MATERIALS_TEXTURES_HARDWARE_TEXTURE_WRAPPER_H
#define BABYLON_MATERIALS_TEXTURES_HARDWARE_TEXTURE_WRAPPER_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

/**
 * @brief Hidden
 */
template <class T>
struct BABYLON_SHARED_EXPORT HardwareTextureWrapper {

  virtual ~HardwareTextureWrapper() = default;

  /**
   * @brief Gets the underlying Resource
   */
  virtual T& underlyingResource() = 0;

  virtual void set(const T& /*hardwareTexture*/) = 0;
  virtual void setUsage(int /*textureSource*/, bool /*generateMipMaps*/, bool /*isCube*/,
                        int /*width*/, int /*height*/)
    = 0;
  virtual void reset()   = 0;
  virtual void release() = 0;

}; // end of struct HardwareTextureWrapper

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_HARDWARE_TEXTURE_WRAPPER_H
