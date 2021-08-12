#ifndef BABYLON_MATERIALS_TEXTURES_ITEXTURE_CREATION_OPTIONS_H
#define BABYLON_MATERIALS_TEXTURES_ITEXTURE_CREATION_OPTIONS_H

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>
#include <babylon/core/array_buffer_view.h>
#include <babylon/core/structs.h>

namespace BABYLON {

FWD_STRUCT_SPTR(LoaderOptions)

/**
 * @brief Defines the available options when creating a texture.
 */
struct BABYLON_SHARED_EXPORT ITextureCreationOptions {
  /**
   *  Defines if the texture will require mip maps or not.
   *  (default: false)
   */
  std::optional<bool> noMipmap = std::nullopt;

  /**
   *  Defines if the texture needs to be inverted on the y axis during loading.
   *  (default: true)
   */
  std::optional<bool> invertY = std::nullopt;

  /**
   *  Defines the sampling mode we want for the texture while fetching from it
   * (Texture.NEAREST_SAMPLINGMODE...)
   * (default: Texture.TRILINEAR_SAMPLINGMODE)
   */
  std::optional<unsigned int> samplingMode = std::nullopt;

  /**
   *  Defines a callback triggered when the texture has been loaded.
   *  (default: null)
   */
  std::function<void()> onLoad = nullptr;

  /**
   *  Defines a callback triggered when an error occurred during the loading session.
   *  (default: null)
   */
  std::function<void(const std::string& message, const std::string& exception)> onError = nullptr;

  /**
   *  Defines the buffer to load the texture from in case the texture is loaded from a buffer
   * representation.
   * (default: null)
   */
  std::optional<std::variant<std::string, ArrayBuffer, ArrayBufferView, Image>> buffer
    = std::nullopt;

  /**
   *  Defines if the buffer we are loading the texture from should be deleted after load
   *  (default: false)
   */
  std::optional<bool> deleteBuffer = std::nullopt;

  /**
   *  Defines the format of the texture we are trying to load (Engine.TEXTUREFORMAT_RGBA...)
   * (default: )
   */
  std::optional<unsigned int> format = std::nullopt;

  /**
   *  Defines an optional mime type information
   *  (default: undefined)
   */
  std::string mimeType = "";

  /**
   *  Options to be passed to the loader
   *  (default: undefined)
   */
  LoaderOptionsPtr loaderOptions = nullptr;

  /**
   *  Specific flags to use when creating the texture (Constants.TEXTURE_CREATIONFLAG_STORAGE for
   * storage textures, for eg)
   * (default: undefined)
   */
  std::optional<unsigned int> creationFlags = std::nullopt;

  /**
   *  Defines if the texture must be loaded in a sRGB GPU buffer (if supported by the GPU)
   *  (default: false)
   */
  std::optional<bool> useSRGBBuffer = std::nullopt;
}; // end of struct ITextureCreationOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_ITEXTURE_CREATION_OPTIONS_H
