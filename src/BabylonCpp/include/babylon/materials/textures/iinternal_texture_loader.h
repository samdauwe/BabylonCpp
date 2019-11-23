#ifndef BABYLON_MATERIALS_TEXTURES_IINTERNAL_TEXTURE_LOADER_H
#define BABYLON_MATERIALS_TEXTURES_IINTERNAL_TEXTURE_LOADER_H

#include <functional>
#include <memory>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/misc/dds_info.h>

namespace BABYLON {

class EventState;
class InternalTexture;
using InternalTexturePtr = std::shared_ptr<InternalTexture>;

struct CubeTextureData {
  bool isDDS;
  int width;
  DDSInfo info;
  std::vector<std::variant<std::string, ArrayBuffer>> data;
  InternalTexturePtr texture;
}; // End of struct CubeTextureData

/**
 * @brief This represents the required contract to create a new type of texture
 * loader.
 */
struct BABYLON_SHARED_EXPORT IInternalTextureLoader {
  virtual ~IInternalTextureLoader() = default;
  /**
   * @brief Defines wether the loader supports cascade loading the different
   * faces.
   */
  [[nodiscard]] virtual bool supportCascades() const = 0;

  /**
   * @brief This returns if the loader support the current file information.
   * @param extension defines the file extension of the file being loaded
   * @param textureFormatInUse defines the current compressed format in use iun
   * the engine
   * @param fallback defines the fallback internal texture if any
   * @param isBase64 defines whether the texture is encoded as a base64
   * @param isBuffer defines whether the texture data are stored as a buffer
   * @returns true if the loader can load the specified file
   */
  virtual bool canLoad(const std::string& extension, const std::string& textureFormatInUse,
                       const InternalTexturePtr& fallback, bool isBase64, bool isBuffer)
    = 0;

  /**
   * @brief Transform the url before loading if required.
   * @param rootUrl the url of the texture
   * @param textureFormatInUse defines the current compressed format in use iun
   * the engine
   * @returns the transformed texture
   */
  virtual std::string transformUrl(const std::string& rootUrl,
                                   const std::string& textureFormatInUse)
    = 0;

  /**
   * @brief Gets the fallback url in case the load fail. This can return null to
   * allow the default fallback mecanism to work
   * @param rootUrl the url of the texture
   * @param textureFormatInUse defines the current compressed format in use iun
   * the engine
   * @returns the fallback texture
   */
  virtual std::string getFallbackTextureUrl(const std::string& rootUrl,
                                            const std::string& textureFormatInUse)
    = 0;

  /**
   * @brief Uploads the cube texture data to the WebGl Texture. It has alreday
   * been bound.
   * @param img contains the texture data
   * @param texture defines the BabylonJS internal texture
   * @param createPolynomials will be true if polynomials have been requested
   * @param onLoad defines the callback to trigger once the texture is ready
   * @param onError defines the callback to trigger in case of error
   */
  virtual void loadCubeData(
    const std::variant<std::string, ArrayBuffer>& img, const InternalTexturePtr& texture,
    bool createPolynomials,
    const std::function<void(const std::optional<CubeTextureData>& data)>& onLoad,
    const std::function<void(const std::string& message, const std::string& exception)>& onError)
    = 0;

  /**
   * @brief Uploads the cube texture data to the WebGl Texture. It has alreday
   * been bound.
   * @param img contains the texture data
   * @param texture defines the BabylonJS internal texture
   * @param createPolynomials will be true if polynomials have been requested
   * @param onLoad defines the callback to trigger once the texture is ready
   * @param onError defines the callback to trigger in case of error
   */
  virtual void loadCubeData(
    const std::vector<std::variant<std::string, ArrayBuffer>>& imgs,
    const InternalTexturePtr& texture, bool createPolynomials,
    const std::function<void(const std::optional<CubeTextureData>& datas)>& onLoad,
    const std::function<void(const std::string& message, const std::string& exception)>& onError)
    = 0;

  /**
   * @brief Uploads the 2D texture data to the WebGl Texture. It has alreday
   * been bound once in the callback.
   * @param data contains the texture data
   * @param texture defines the BabylonJS internal texture
   * @param callback defines the method to call once ready to upload
   */
  virtual void
  loadData(const ArrayBuffer& data, const InternalTexturePtr& texture,
           const std::function<void(int width, int height, bool loadMipmap, bool isCompressed,
                                    const std::function<void()>& done, bool loadFailed)>& callback)
    = 0;

}; // end of struct IInternalTextureLoader

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_IINTERNAL_TEXTURE_LOADER_H
