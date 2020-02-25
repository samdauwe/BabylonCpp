#ifndef BABYLON_MATERIALS_TEXTURES_LOADERS_ENV_TEXTURE_LOADER_H
#define BABYLON_MATERIALS_TEXTURES_LOADERS_ENV_TEXTURE_LOADER_H

#include <babylon/materials/textures/iinternal_texture_loader.h>

namespace BABYLON {

/**
 * @brief Implementation of the ENV Texture Loader.
 */
struct BABYLON_SHARED_EXPORT _ENVTextureLoader : public IInternalTextureLoader {

  _ENVTextureLoader();
  ~_ENVTextureLoader() override; // = default

  /**
   * @brief Defines wether the loader supports cascade loading the different faces.
   */
  [[nodiscard]] bool supportCascades() const override;

  /**
   * @brief This returns if the loader support the current file information.
   * @param extension defines the file extension of the file being loaded
   * @param textureFormatInUse defines the current compressed format in use iun the engine
   * @param fallback defines the fallback internal texture if any
   * @param isBase64 defines whether the texture is encoded as a base64
   * @param isBuffer defines whether the texture data are stored as a buffer
   * @returns true if the loader can load the specified file
   */
  bool canLoad(const std::string& extension, const std::string& textureFormatInUse,
               const InternalTexturePtr& fallback, bool isBase64, bool isBuffer) override;

  /**
   * @brief Transform the url before loading if required.
   * @param rootUrl the url of the texture
   * @param textureFormatInUse defines the current compressed format in use iun the engine
   * @returns the transformed texture
   */
  std::string transformUrl(const std::string& rootUrl,
                           const std::string& textureFormatInUse) override;

  /**
   * @brief Gets the fallback url in case the load fail. This can return null to allow the default
   * fallback mecanism to work
   * @param rootUrl the url of the texture
   * @param textureFormatInUse defines the current compressed format in use iun the engine
   * @returns the fallback texture
   */
  std::string getFallbackTextureUrl(const std::string& rootUrl,
                                    const std::string& textureFormatInUse) override;

  /**
   * @brief Uploads the cube texture data to the WebGl Texture. It has alreday been bound.
   * @param data contains the texture data
   * @param texture defines the BabylonJS internal texture
   * @param createPolynomials will be true if polynomials have been requested
   * @param onLoad defines the callback to trigger once the texture is ready
   * @param onError defines the callback to trigger in case of error
   */
  void loadCubeData(const std::variant<std::string, ArrayBufferView>& data,
                    const InternalTexturePtr& texture, bool createPolynomials,
                    const std::function<void(const std::optional<CubeTextureData>& data)>& onLoad,
                    const std::function<void(const std::string& message,
                                             const std::string& exception)>& onError) override;

  /**
   * @brief Uploads the 2D texture data to the WebGl Texture. It has alreday been bound once in the
   * callback.
   * @param data contains the texture data
   * @param texture defines the BabylonJS internal texture
   * @param callback defines the method to call once ready to upload
   */
  void loadCubeData(const std::vector<std::variant<std::string, ArrayBufferView>>& data,
                    const InternalTexturePtr& texture, bool createPolynomials,
                    const std::function<void(const std::optional<CubeTextureData>& data)>& onLoad,
                    const std::function<void(const std::string& message,
                                             const std::string& exception)>& onError) override;

  /**
   * @brief Uploads the 2D texture data to the WebGl Texture. It has already been bound once in the
   * callback.
   * @param data contains the texture data
   * @param texture defines the BabylonJS internal texture
   * @param callback defines the method to call once ready to upload
   */
  void loadData(const ArrayBufferView& data, const InternalTexturePtr& texture,
                const std::function<void(int width, int height, bool loadMipmap, bool isCompressed,
                                         const std::function<void()>& done, bool loadFailed)>&
                  callback) override;

}; // end of struct _ENVTextureLoader

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_LOADERS_ENV_TEXTURE_LOADER_H
