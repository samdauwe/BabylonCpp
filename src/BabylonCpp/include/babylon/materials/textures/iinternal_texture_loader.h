#ifndef BABYLON_MATERIALS_TEXTURES_IINTERNAL_TEXTURE_LOADER_H
#define BABYLON_MATERIALS_TEXTURES_IINTERNAL_TEXTURE_LOADER_H

#include <functional>
#include <memory>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>
#include <babylon/core/array_buffer_view.h>
#include <babylon/misc/dds_info.h>

namespace BABYLON {

class EventState;
FWD_CLASS_SPTR(InternalTexture)

struct CubeTextureData {
  bool isDDS;
  int width;
  DDSInfo info;
  std::vector<std::variant<std::string, ArrayBufferView>> data;
  InternalTexturePtr texture;
}; // End of struct CubeTextureData

/**
 * @brief This represents the required contract to create a new type of texture loader.
 */
struct BABYLON_SHARED_EXPORT IInternalTextureLoader {
  virtual ~IInternalTextureLoader() = default;
  /**
   * @brief Defines whether the loader supports cascade loading the different faces.
   */
  virtual bool supportCascades() const = 0;

  /**
   * @brief This returns if the loader support the current file information.
   * @param extension defines the file extension of the file being loaded
   * @param mimeType defines the optional mime type of the file being loaded
   * @returns true if the loader can load the specified file
   */
  virtual bool canLoad(const std::string& extension, const std::string& mimeType = "") = 0;

  /**
   * @brief Uploads the cube texture data to the WebGL texture. It has already been bound.
   * @param data contains the texture data
   * @param texture defines the BabylonJS internal texture
   * @param createPolynomials will be true if polynomials have been requested
   * @param onLoad defines the callback to trigger once the texture is ready
   * @param onError defines the callback to trigger in case of error
   */
  virtual void loadCubeData(
    const std::variant<std::string, ArrayBufferView>& img, const InternalTexturePtr& texture,
    bool createPolynomials,
    const std::function<void(const std::optional<CubeTextureData>& data)>& onLoad,
    const std::function<void(const std::string& message, const std::string& exception)>& onError)
    = 0;

  /**
   * @brief Uploads the cube texture data to the WebGL texture. It has already been bound.
   * @param data contains the texture data
   * @param texture defines the BabylonJS internal texture
   * @param createPolynomials will be true if polynomials have been requested
   * @param onLoad defines the callback to trigger once the texture is ready
   * @param onError defines the callback to trigger in case of error
   */
  virtual void loadCubeData(
    const std::vector<std::variant<std::string, ArrayBufferView>>& imgs,
    const InternalTexturePtr& texture, bool createPolynomials,
    const std::function<void(const std::optional<CubeTextureData>& datas)>& onLoad,
    const std::function<void(const std::string& message, const std::string& exception)>& onError)
    = 0;

  /**
   * @brief Uploads the 2D texture data to the WebGL texture. It has already been bound once in the
   * callback.
   * @param data contains the texture data
   * @param texture defines the BabylonJS internal texture
   * @param callback defines the method to call once ready to upload
   */
  virtual void
  loadData(const ArrayBufferView& data, const InternalTexturePtr& texture,
           const std::function<void(int width, int height, bool loadMipmap, bool isCompressed,
                                    const std::function<void()>& done, bool loadFailed)>& callback)
    = 0;

}; // end of struct IInternalTextureLoader

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_IINTERNAL_TEXTURE_LOADER_H
