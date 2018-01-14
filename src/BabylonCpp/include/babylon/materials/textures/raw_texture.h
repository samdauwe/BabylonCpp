#ifndef BABYLON_MATERIALS_TEXTURES_RAW_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_RAW_TEXTURE_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/texture.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT RawTexture : public Texture {

public:
  RawTexture(const Uint8Array& data, int width, int height, unsigned int format,
             Scene* scene, bool generateMipMaps = true, bool invertY = false,
             unsigned int samplingMode
             = TextureConstants::TRILINEAR_SAMPLINGMODE,
             unsigned int type = EngineConstants::TEXTURETYPE_UNSIGNED_INT);
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

  static unique_ptr_t<RawTexture> CreateRGBTexture(
    const Uint8Array& data, int width, int height, Scene* scene,
    bool generateMipMaps = true, bool invertY = false,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
    unsigned int type         = EngineConstants::TEXTURETYPE_UNSIGNED_INT);

  static unique_ptr_t<RawTexture> CreateRGBATexture(
    const Uint8Array& data, int width, int height, Scene* scene,
    bool generateMipMaps = true, bool invertY = false,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
    unsigned int type         = EngineConstants::TEXTURETYPE_UNSIGNED_INT);

  unsigned int format;
  unsigned int wrapU;
  unsigned int wrapV;
  unsigned int type;

private:
  Engine* _engine;

}; // struct of class RawTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_RAW_TEXTURE_H
