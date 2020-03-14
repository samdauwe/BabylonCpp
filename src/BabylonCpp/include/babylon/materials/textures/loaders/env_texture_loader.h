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
  bool supportCascades() const override;

  /**
   * @brief This returns if the loader support the current file information.
   * @param extension defines the file extension of the file being loaded
   * @returns true if the loader can load the specified file
   */
  bool canLoad(const std::string& extension) override;

  /**
   * @brief Uploads the cube texture data to the WebGL texture. It has already been bound.
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
   * @brief Uploads the cube texture data to the WebGL texture. It has already been bound.
   * @param data contains the texture data
   * @param texture defines the BabylonJS internal texture
   * @param createPolynomials will be true if polynomials have been requested
   * @param onLoad defines the callback to trigger once the texture is ready
   * @param onError defines the callback to trigger in case of error
   */
  void loadCubeData(const std::vector<std::variant<std::string, ArrayBufferView>>& data,
                    const InternalTexturePtr& texture, bool createPolynomials,
                    const std::function<void(const std::optional<CubeTextureData>& data)>& onLoad,
                    const std::function<void(const std::string& message,
                                             const std::string& exception)>& onError) override;

  /**
   * @brief Uploads the 2D texture data to the WebGL texture. It has already been bound once in the
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
