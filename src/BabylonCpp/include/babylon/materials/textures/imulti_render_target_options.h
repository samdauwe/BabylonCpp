#ifndef BABYLON_MATERIALS_TEXTURES_IMULTI_RENDER_TARGET_OPTIONS_H
#define BABYLON_MATERIALS_TEXTURES_IMULTI_RENDER_TARGET_OPTIONS_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

/**
 * @brief Creation options of the multi render target texture.
 */
struct BABYLON_SHARED_EXPORT IMultiRenderTargetOptions {

  /**
   * Define if the texture needs to create mip maps after render.
   */
  std::optional<bool> generateMipMaps = std::nullopt;

  /**
   * Define the types of all the draw buffers we want to create
   */
  std::vector<unsigned int> types = {};

  /**
   * Define the sampling modes of all the draw buffers we want to create
   */
  std::vector<unsigned int> samplingModes = {};

  /**
   * Define if a depth buffer is required
   */
  std::optional<bool> generateDepthBuffer = std::nullopt;

  /**
   * Define if a stencil buffer is required
   */
  std::optional<bool> generateStencilBuffer = std::nullopt;

  /**
   * Define if a depth texture is required instead of a depth buffer
   */
  std::optional<bool> generateDepthTexture = std::nullopt;

  /**
   * Define the number of desired draw buffers
   */
  std::optional<size_t> textureCount = std::nullopt;

  /**
   * Define if aspect ratio should be adapted to the texture or stay the scene
   * one
   */
  std::optional<bool> doNotChangeAspectRatio = std::nullopt;

  /**
   * Define the default type of the buffers we are creating
   */
  std::optional<unsigned int> defaultType = std::nullopt;

  /**
   * Define the default type of the buffers we are creating
   */
  std::optional<bool> drawOnlyOnFirstAttachmentByDefault = std::nullopt;

}; // end of struct IMultiRenderTargetOptions

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_TEXTURES_IMULTI_RENDER_TARGET_OPTIONS_H
