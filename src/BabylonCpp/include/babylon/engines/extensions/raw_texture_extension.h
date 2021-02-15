#ifndef BABYLON_ENGINES_EXTENSIONS_RAW_TEXTURE_EXTENSION_H
#define BABYLON_ENGINES_EXTENSIONS_RAW_TEXTURE_EXTENSION_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/engines/constants.h>
#include <babylon/materials/textures/texture_constants.h>

namespace BABYLON {

class ArrayBufferView;
using ArrayBufferViewArray = std::vector<ArrayBufferView>;
class InternalTexture;
class Scene;
class ThinEngine;
using InternalTexturePtr = std::shared_ptr<InternalTexture>;

/**
 * @brief Hidden
 */
class BABYLON_SHARED_EXPORT RawTextureExtension {

public:
  RawTextureExtension(ThinEngine* engine);
  ~RawTextureExtension();

  /**
   * @brief Creates a raw texture.
   * @param data defines the data to store in the texture
   * @param width defines the width of the texture
   * @param height defines the height of the texture
   * @param format defines the format of the data
   * @param generateMipMaps defines if the engine should generate the mip levels
   * @param invertY defines if data must be stored with Y axis inverted
   * @param samplingMode defines the required sampling mode (Texture.NEAREST_SAMPLINGMODE by
   * default)
   * @param compression defines the compression used (null by default)
   * @param type defines the type fo the data (Engine.TEXTURETYPE_UNSIGNED_INT by default)
   * @returns the raw texture inside an InternalTexture
   */
  InternalTexturePtr createRawTexture(const Uint8Array& data, int width, int height,
                                      unsigned int format, bool generateMipMaps, bool invertY,
                                      unsigned int samplingMode,
                                      const std::string& compression = "",
                                      unsigned int type = Constants::TEXTURETYPE_UNSIGNED_INT);

  /**
   * @brief Update a raw texture.
   * @param texture defines the texture to update
   * @param data defines the data to store in the texture
   * @param format defines the format of the data
   * @param invertY defines if data must be stored with Y axis inverted
   * @param compression defines the compression used (null by default)
   * @param type defines the type fo the data (Engine.TEXTURETYPE_UNSIGNED_INT by default)
   */
  void updateRawTexture(const InternalTexturePtr& texture, const Uint8Array& data,
                        unsigned int format, bool invertY = true,
                        const std::string& compression = "",
                        unsigned int type              = Constants::TEXTURETYPE_UNSIGNED_INT);

  /**
   * @brief Creates a new raw cube texture.
   * @param data defines the array of data to use to create each face
   * @param size defines the size of the textures
   * @param format defines the format of the data
   * @param type defines the type of the data (like Engine.TEXTURETYPE_UNSIGNED_INT)
   * @param generateMipMaps  defines if the engine should generate the mip levels
   * @param invertY defines if data must be stored with Y axis inverted
   * @param samplingMode defines the required sampling mode (like Texture.NEAREST_SAMPLINGMODE)
   * @param compression defines the compression used (null by default)
   * @returns the cube texture as an InternalTexture
   */
  InternalTexturePtr createRawCubeTexture(const std::vector<ArrayBufferView>& data, int size,
                                          unsigned int format, unsigned int type,
                                          bool generateMipMaps, bool invertY,
                                          unsigned int samplingMode,
                                          const std::string& compression = "");

  /**
   * @brief Update a raw cube texture.
   * @param texture defines the texture to update
   * @param data defines the data to store
   * @param format defines the data format
   * @param type defines the type fo the data (Engine.TEXTURETYPE_UNSIGNED_INT by default)
   * @param invertY defines if data must be stored with Y axis inverted
   * @param compression defines the compression used (null by default)
   * @param level defines which level of the texture to update
   */
  void updateRawCubeTexture(const InternalTexturePtr& texture,
                            const std::vector<ArrayBufferView>& data, unsigned int format,
                            unsigned int type, bool invertY = true,
                            const std::string& compression = "", unsigned int level = 0);

  /**
   * @brief Creates a new raw cube texture from a specified url.
   * @param url defines the url where the data is located
   * @param scene defines the current scene
   * @param size defines the size of the textures
   * @param format defines the format of the data
   * @param type defines the type fo the data (like Engine.TEXTURETYPE_UNSIGNED_INT)
   * @param noMipmap defines if the engine should avoid generating the mip levels
   * @param callback defines a callback used to extract texture data from loaded data
   * @param mipmapGenerator defines to provide an optional tool to generate mip levels
   * @param onLoad defines a callback called when texture is loaded
   * @param onError defines a callback called if there is an error
   * @param samplingMode defines the required sampling mode (like Texture.NEAREST_SAMPLINGMODE)
   * @param invertY defines if data must be stored with Y axis inverted
   * @returns the cube texture as an InternalTexture
   */
  InternalTexturePtr createRawCubeTextureFromUrl(
    const std::string& url, Scene* scene, int size, unsigned int format, unsigned int type,
    bool noMipmap,
    const std::function<ArrayBufferViewArray(const ArrayBuffer& arrayBuffer)>& callback,
    const std::function<std::vector<ArrayBufferViewArray>(const ArrayBufferViewArray& faces)>&
      mipmapGenerator,
    const std::function<void()>& onLoad = nullptr,
    const std::function<void(const std::string& message, const std::string& exception)>& onError
    = nullptr,
    unsigned int samplingMode = TextureConstants::TRILINEAR_SAMPLINGMODE, bool invertY = false);

  /**
   * @brief Creates a new raw 3D texture.
   * @param data defines the data used to create the texture
   * @param width defines the width of the texture
   * @param height defines the height of the texture
   * @param depth defines the depth of the texture
   * @param format defines the format of the texture
   * @param generateMipMaps defines if the engine must generate mip levels
   * @param invertY defines if data must be stored with Y axis inverted
   * @param samplingMode defines the required sampling mode (like Texture.NEAREST_SAMPLINGMODE)
   * @param compression defines the compressed used (can be null)
   * @param textureType defines the compressed used (can be null)
   * @returns a new raw 3D texture (stored in an InternalTexture)
   */
  InternalTexturePtr
  createRawTexture3D(const ArrayBufferView& data, int width, int height, int depth,
                     unsigned int format, bool generateMipMaps, bool invertY,
                     unsigned int samplingMode, const std::string& compression = "",
                     unsigned int textureType = Constants::TEXTURETYPE_UNSIGNED_INT);

  /**
   * @brief Update a raw 3D texture.
   * @param texture defines the texture to update
   * @param data defines the data to store
   * @param format defines the data format
   * @param invertY defines if data must be stored with Y axis inverted
   * @param compression defines the used compression (can be null)
   * @param textureType defines the texture Type (Engine.TEXTURETYPE_UNSIGNED_INT,
   * Engine.TEXTURETYPE_FLOAT...)
   */
  void updateRawTexture3D(const InternalTexturePtr& texture, const ArrayBufferView& data,
                          unsigned int format, bool invertY = true,
                          const std::string& compression = "",
                          unsigned int textureType       = Constants::TEXTURETYPE_UNSIGNED_INT);

  /**
   * @brief Creates a new raw 2D array texture.
   * @param data defines the data used to create the texture
   * @param width defines the width of the texture
   * @param height defines the height of the texture
   * @param depth defines the number of layers of the texture
   * @param format defines the format of the texture
   * @param generateMipMaps defines if the engine must generate mip levels
   * @param invertY defines if data must be stored with Y axis inverted
   * @param samplingMode defines the required sampling mode (like Texture.NEAREST_SAMPLINGMODE)
   * @param compression defines the compressed used (can be null)
   * @param textureType defines the compressed used (can be null)
   * @returns a new raw 2D array texture (stored in an InternalTexture)
   */
  InternalTexturePtr
  createRawTexture2DArray(const ArrayBufferView& data, int width, int height, int depth,
                          unsigned int format, bool generateMipMaps, bool invertY,
                          unsigned int samplingMode, const std::string& compression = "",
                          unsigned int textureType = Constants::TEXTURETYPE_UNSIGNED_INT);

  /**
   * @brief Update a raw 2D array texture.
   * @param texture defines the texture to update
   * @param data defines the data to store
   * @param format defines the data format
   * @param invertY defines if data must be stored with Y axis inverted
   * @param compression defines the used compression (can be null)
   * @param textureType defines the texture Type (Engine.TEXTURETYPE_UNSIGNED_INT,
   * Engine.TEXTURETYPE_FLOAT...)
   */
  void updateRawTexture2DArray(const InternalTexturePtr& texture, const ArrayBufferView& data,
                               unsigned int format, bool invertY = true,
                               const std::string& compression = "",
                               unsigned int textureType = Constants::TEXTURETYPE_UNSIGNED_INT);

private:
  /**
   * @brief Create a texture for createRawTexture3D/createRawTexture2DArray
   * @param data defines the data used to create the texture
   * @param width defines the width of the texture
   * @param height defines the height of the texture
   * @param depth defines the number of layers of the texture
   * @param format defines the format of the texture
   * @param generateMipMaps defines if the engine must generate mip levels
   * @param invertY defines if data must be stored with Y axis inverted
   * @param samplingMode defines the required sampling mode (like Texture.NEAREST_SAMPLINGMODE)
   * @param compression defines the compressed used (can be null)
   * @param textureType defines the compressed used (can be null)
   * @param is3D true for TEXTURE_3D and false for TEXTURE_2D_ARRAY
   * @hidden
   */
  InternalTexturePtr
  _createRawTexture(const ArrayBufferView& data, int width, int height, int depth,
                    unsigned int format, bool generateMipMaps, bool invertY,
                    unsigned int samplingMode, const std::string& compression = "",
                    unsigned int textureType = Constants::TEXTURETYPE_UNSIGNED_INT,
                    bool is3D                = false);

  /**
   * @brief Updates a texture for updateRawTexture3D/updateRawTexture2DArray
   * @param texture defines the texture to update
   * @param data defines the data to store
   * @param format defines the data format
   * @param invertY defines if data must be stored with Y axis inverted
   * @param compression defines the used compression (can be null)
   * @param textureType defines the texture Type (Engine.TEXTURETYPE_UNSIGNED_INT,
   * Engine.TEXTURETYPE_FLOAT...)
   * @param is3D true for TEXTURE_3D and false for TEXTURE_2D_ARRAY
   * @hidden
   */
  void _updateRawTexture(const InternalTexturePtr& texture, const ArrayBufferView& data,
                         unsigned int format, bool invertY = true,
                         const std::string& compression = "",
                         unsigned int textureType       = Constants::TEXTURETYPE_UNSIGNED_INT,
                         bool is3D                      = false);

  /**
   * @brief Hidden
   */
  ArrayBufferView _convertRGBtoRGBATextureData(const ArrayBufferView& rgbData, int width,
                                               int height, unsigned int textureType);

private:
  ThinEngine* _this;

}; // end of class RawTextureExtension

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_EXTENSIONS_RAW_TEXTURE_EXTENSION_H
