#ifndef BABYLON_MATERIALS_TEXTURES_RAW_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_RAW_TEXTURE_H

#include <babylon/babylon_api.h>
#include <babylon/engines/constants.h>
#include <babylon/materials/textures/texture.h>

namespace BABYLON {

class Engine;

/**
 * @brief Raw texture can help creating a texture directly from an array of
 * data. This can be super useful if you either get the data from an
 * uncompressed source or if you wish to create your texture pixel by pixel.
 */
class BABYLON_SHARED_EXPORT RawTexture : public Texture {

public:
  /**
   * @brief Instantiates a new RawTexture.
   * Raw texture can help creating a texture directly from an array of data.
   * This can be super useful if you either get the data from an uncompressed
   * source or if you wish to create your texture pixel by pixel.
   * @param data define the array of data to use to create the texture
   * @param width define the width of the texture
   * @param height define the height of the texture
   * @param format define the format of the data (RGB, RGBA...
   * Engine.TEXTUREFORMAT_xxx)
   * @param scene  define the scene the texture belongs to
   * @param generateMipMaps define whether mip maps should be generated or not
   * @param invertY define if the data should be flipped on Y when uploaded to
   * the GPU
   * @param samplingMode define the texture sampling mode
   * (Texture.xxx_SAMPLINGMODE)
   * @param type define the format of the data (int, float...
   * Engine.TEXTURETYPE_xxx)
   */
  RawTexture(const ArrayBufferView& data, int width, int height,
             unsigned int format, Scene* scene, bool generateMipMaps = true,
             bool invertY = false,
             unsigned int samplingMode
             = Constants::TEXTURE_TRILINEAR_SAMPLINGMODE,
             unsigned int type = Constants::TEXTURETYPE_UNSIGNED_INT);
  ~RawTexture(); // = default

  /**
   * @brief Updates the texture underlying data.
   * @param data Define the new data of the texture
   */
  void update(const ArrayBufferView& data);

  /**
   * @brief Creates a luminance texture from some data.
   * @param data Define the texture data
   * @param width Define the width of the texture
   * @param height Define the height of the texture
   * @param scene Define the scene the texture belongs to
   * @param generateMipMaps Define whether or not to create mip maps for the
   * texture
   * @param invertY define if the data should be flipped on Y when uploaded to
   * the GPU
   * @param samplingMode define the texture sampling mode
   * (Texture.xxx_SAMPLINGMODE)
   * @returns the luminance texture
   */
  static std::unique_ptr<RawTexture> CreateLuminanceTexture(
    const ArrayBufferView& data, int width, int height, Scene* scene,
    bool generateMipMaps = true, bool invertY = false,
    unsigned int samplingMode = Constants::TEXTURE_TRILINEAR_SAMPLINGMODE);

  /**
   * @brief Creates a luminance alpha texture from some data.
   * @param data Define the texture data
   * @param width Define the width of the texture
   * @param height Define the height of the texture
   * @param scene Define the scene the texture belongs to
   * @param generateMipMaps Define whether or not to create mip maps for the
   * texture
   * @param invertY define if the data should be flipped on Y when uploaded to
   * the GPU
   * @param samplingMode define the texture sampling mode
   * (Texture.xxx_SAMPLINGMODE)
   * @returns the luminance alpha texture
   */
  static std::unique_ptr<RawTexture> CreateLuminanceAlphaTexture(
    const ArrayBufferView& data, int width, int height, Scene* scene,
    bool generateMipMaps = true, bool invertY = false,
    unsigned int samplingMode = Constants::TEXTURE_TRILINEAR_SAMPLINGMODE);

  /**
   * @brief Creates an alpha texture from some data.
   * @param data Define the texture data
   * @param width Define the width of the texture
   * @param height Define the height of the texture
   * @param scene Define the scene the texture belongs to
   * @param generateMipMaps Define whether or not to create mip maps for the
   * texture
   * @param invertY define if the data should be flipped on Y when uploaded to
   * the GPU
   * @param samplingMode define the texture sampling mode
   * (Texture.xxx_SAMPLINGMODE)
   * @returns the alpha texture
   */
  static std::unique_ptr<RawTexture> CreateAlphaTexture(
    const ArrayBufferView& data, int width, int height, Scene* scene,
    bool generateMipMaps = true, bool invertY = false,
    unsigned int samplingMode = Constants::TEXTURE_TRILINEAR_SAMPLINGMODE);

  /**
   * @brief Creates a RGB texture from some data.
   * @param data Define the texture data
   * @param width Define the width of the texture
   * @param height Define the height of the texture
   * @param scene Define the scene the texture belongs to
   * @param generateMipMaps Define whether or not to create mip maps for the
   * texture
   * @param invertY define if the data should be flipped on Y when uploaded to
   * the GPU
   * @param samplingMode define the texture sampling mode
   * (Texture.xxx_SAMPLINGMODE)
   * @param type define the format of the data (int, float...
   * Engine.TEXTURETYPE_xxx)
   * @returns the RGB alpha texture
   */
  static std::unique_ptr<RawTexture> CreateRGBTexture(
    const ArrayBufferView& data, int width, int height, Scene* scene,
    bool generateMipMaps = true, bool invertY = false,
    unsigned int samplingMode = Constants::TEXTURE_TRILINEAR_SAMPLINGMODE,
    unsigned int type         = Constants::TEXTURETYPE_UNSIGNED_INT);

  /**
   * @brief Creates a RGBA texture from some data.
   * @param data Define the texture data
   * @param width Define the width of the texture
   * @param height Define the height of the texture
   * @param scene Define the scene the texture belongs to
   * @param generateMipMaps Define whether or not to create mip maps for the
   * texture
   * @param invertY define if the data should be flipped on Y when uploaded to
   * the GPU
   * @param samplingMode define the texture sampling mode
   * (Texture.xxx_SAMPLINGMODE)
   * @param type define the format of the data (int, float...
   * Engine.TEXTURETYPE_xxx)
   * @returns the RGBA texture
   */
  static std::unique_ptr<RawTexture> CreateRGBATexture(
    const ArrayBufferView& data, int width, int height, Scene* scene,
    bool generateMipMaps = true, bool invertY = false,
    unsigned int samplingMode = Constants::TEXTURE_TRILINEAR_SAMPLINGMODE,
    unsigned int type         = Constants::TEXTURETYPE_UNSIGNED_INT);

  /**
   * @brief Creates a R texture from some data.
   * @param data Define the texture data
   * @param width Define the width of the texture
   * @param height Define the height of the texture
   * @param scene Define the scene the texture belongs to
   * @param generateMipMaps Define whether or not to create mip maps for the
   * texture
   * @param invertY define if the data should be flipped on Y when uploaded to
   * the GPU
   * @param samplingMode define the texture sampling mode
   * (Texture.xxx_SAMPLINGMODE)
   * @param type define the format of the data (int, float...
   * Engine.TEXTURETYPE_xxx)
   * @returns the R texture
   */
  static std::unique_ptr<RawTexture> CreateRTexture(
    const ArrayBufferView& data, int width, int height, Scene* scene,
    bool generateMipMaps = true, bool invertY = false,
    unsigned int samplingMode = Constants::TEXTURE_TRILINEAR_SAMPLINGMODE,
    unsigned int type         = Constants::TEXTURETYPE_FLOAT);

public:
  /**
   * Define the format of the data (RGB, RGBA... Engine.TEXTUREFORMAT_xxx)
   */
  unsigned int format;

  /**
   * Define the format of the data (int, float... Engine.TEXTURETYPE_xxx)
   */
  unsigned int type;

private:
  Engine* _engine;

}; // struct of class RawTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_RAW_TEXTURE_H
