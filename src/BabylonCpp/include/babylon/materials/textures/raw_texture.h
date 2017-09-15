#ifndef BABYLON_MATERIALS_TEXTURES_RAW_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_RAW_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/texture.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT RawTexture : public Texture {

  RawTexture(const Uint8Array& data, int width, int height, int format,
             Scene* scene, bool generateMipMaps = true, bool invertY = false,
             unsigned int samplingMode
             = TextureConstants::TRILINEAR_SAMPLINGMODE);
  ~RawTexture();

  void update(const Uint8Array& data);

  // Statics
  static unique_ptr_t<RawTexture> CreateLuminanceTexture(
    const Uint8Array& data, int width, int height, Scene* scene,
    bool generateMipMaps = true, bool invertY = false,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE);

  static unique_ptr_t<RawTexture> CreateLuminanceAlphaTexture(
    const Uint8Array& data, int width, int height, Scene* scene,
    bool generateMipMaps = true, bool invertY = false,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE);

  static unique_ptr_t<RawTexture> CreateAlphaTexture(
    const Uint8Array& data, int width, int height, Scene* scene,
    bool generateMipMaps = true, bool invertY = false,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE);

  static unique_ptr_t<RawTexture>
  CreateRGBTexture(const Uint8Array& data, int width, int height, Scene* scene,
                   bool generateMipMaps = true, bool invertY = false,
                   unsigned int samplingMode
                   = TextureConstants::TRILINEAR_SAMPLINGMODE);

  static unique_ptr_t<RawTexture>
  CreateRGBATexture(const Uint8Array& data, int width, int height, Scene* scene,
                    bool generateMipMaps = true, bool invertY = false,
                    unsigned int samplingMode
                    = TextureConstants::TRILINEAR_SAMPLINGMODE);

  int format;
  unsigned int wrapU;
  unsigned int wrapV;

}; // struct of class RawTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_RAW_TEXTURE_H
