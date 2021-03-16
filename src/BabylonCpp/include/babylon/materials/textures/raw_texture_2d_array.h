#ifndef BABYLON_MATERIALS_TEXTURES_RAW_TEXTURE_2D_ARRAY_H
#define BABYLON_MATERIALS_TEXTURES_RAW_TEXTURE_2D_ARRAY_H

#include <babylon/babylon_api.h>
#include <babylon/engines/constants.h>
#include <babylon/materials/textures/texture.h>

namespace BABYLON {

/**
 * @brief Class used to store 2D array textures containing user data.
 */
class BABYLON_SHARED_EXPORT RawTexture2DArray : public Texture {

public:
  /**
   * @brief Create a new RawTexture2DArray.
   * @param data defines the data of the texture
   * @param width defines the width of the texture
   * @param height defines the height of the texture
   * @param depth defines the number of layers of the texture
   * @param format defines the texture format to use
   * @param scene defines the hosting scene
   * @param generateMipMaps defines a boolean indicating if mip levels should be generated (true by
   * default)
   * @param invertY defines if texture must be stored with Y axis inverted
   * @param samplingMode defines the sampling mode to use (Texture.TRILINEAR_SAMPLINGMODE by
   * default)
   * @param textureType defines the texture Type (Engine.TEXTURETYPE_UNSIGNED_INT,
   * Engine.TEXTURETYPE_FLOAT...)
   */
  RawTexture2DArray(const ArrayBufferView& data, int width, int height, int depth,
                    unsigned int format, Scene* scene, bool generateMipMaps = true,
                    bool invertY              = false,
                    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE,
                    unsigned int textureType  = Constants::TEXTURETYPE_UNSIGNED_INT);
  ~RawTexture2DArray() override; // = default

  /**
   * @brief Update the texture with new data.
   * @param data defines the data to store in the texture
   */
  void update(const ArrayBufferView& data);

  /**
   * @brief Creates a RGBA texture from some data.
   * @param data Define the texture data
   * @param width Define the width of the texture
   * @param height Define the height of the texture
   * @param depth defines the number of layers of the texture
   * @param scene defines the scene the texture will belong to
   * @param generateMipMaps Define whether or not to create mip maps for the texture
   * @param invertY define if the data should be flipped on Y when uploaded to the GPU
   * @param samplingMode define the texture sampling mode (Texture.xxx_SAMPLINGMODE)
   * @param type define the format of the data (int, float... Engine.TEXTURETYPE_xxx)
   * @returns the RGBA texture
   */
  static std::unique_ptr<RawTexture2DArray>
  CreateRGBATexture(const ArrayBufferView& data, int width, int height, int depth, Scene* scene,
                    bool generateMipMaps = true, bool invertY = false,
                    unsigned int samplingMode = Constants::TEXTURE_TRILINEAR_SAMPLINGMODE,
                    unsigned int type         = Constants::TEXTURETYPE_UNSIGNED_INT);

protected:
  /**
   * @brief Gets the number of layers of the texture.
   */
  int get_depth() const;

public:
  /**
   * Gets or sets the texture format to use
   */
  unsigned int format;

  /**
   * Gets the number of layers of the texture
   */
  ReadOnlyProperty<RawTexture2DArray, int> depth;

private:
  int _depth;

}; // end of struct of class RawTexture2DArray

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_RAW_TEXTURE_2D_ARRAY_H
