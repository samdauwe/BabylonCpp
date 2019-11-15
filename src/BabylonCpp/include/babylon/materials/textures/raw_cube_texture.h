#ifndef BABYLON_MATERIALS_TEXTURES_RAW_CUBE_TEXTURE_H
#define BABYLON_MATERIALS_TEXTURES_RAW_CUBE_TEXTURE_H

#include <babylon/babylon_api.h>
#include <babylon/engines/constants.h>
#include <babylon/materials/textures/cube_texture.h>
#include <babylon/materials/textures/texture_constants.h>

namespace BABYLON {

/**
 * @brief Raw cube texture where the raw buffers are passed in.
 */
struct BABYLON_SHARED_EXPORT RawCubeTexture : public CubeTexture {

  /**
   * @brief Creates a cube texture where the raw buffers are passed in.
   * @param scene defines the scene the texture is attached to
   * @param data defines the array of data to use to create each face
   * @param size defines the size of the textures
   * @param format defines the format of the data
   * @param type defines the type of the data (like
   * Engine.TEXTURETYPE_UNSIGNED_INT)
   * @param generateMipMaps  defines if the engine should generate the mip
   * levels
   * @param invertY defines if data must be stored with Y axis inverted
   * @param samplingMode defines the required sampling mode (like
   * Texture.NEAREST_SAMPLINGMODE)
   * @param compression defines the compression used (null by default)
   */
  RawCubeTexture(Scene* scene, const std::vector<ArrayBufferView>& data,
                 int size, unsigned int format = Constants::TEXTUREFORMAT_RGBA,
                 unsigned int type    = Constants::TEXTURETYPE_UNSIGNED_INT,
                 bool generateMipMaps = false, bool invertY = false,
                 unsigned int samplingMode
                 = Constants::TEXTURE_TRILINEAR_SAMPLINGMODE,
                 const std::string& compression = "");
  ~RawCubeTexture(); // = default

  /**
   * @brief Updates the raw cube texture.
   * @param data defines the data to store
   * @param format defines the data format
   * @param type defines the type fo the data (Engine.TEXTURETYPE_UNSIGNED_INT
   * by default)
   * @param invertY defines if data must be stored with Y axis inverted
   * @param compression defines the compression used (null by default)
   * @param level defines which level of the texture to update
   */
  void update(const std::vector<ArrayBufferView>& data, unsigned int format,
              unsigned int type, bool invertY,
              const std::string& compression = "", unsigned int level = 0);

  /**
   * @brief Clones the raw cube texture.
   * @return a new cube texture
   */
  CubeTexture* clone() const;

}; // struct of class RawCubeTexture

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_RAW_CUBE_TEXTURE_H
