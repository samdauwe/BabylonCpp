#ifndef BABYLON_ENGINES_RENDER_TARGET_WRAPPER_H
#define BABYLON_ENGINES_RENDER_TARGET_WRAPPER_H

#include <variant>
#include <vector>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>
#include <babylon/core/structs.h>

namespace BABYLON {

class ThinEngine;
FWD_CLASS_SPTR(InternalTexture)

/**
 * @brief Wrapper around a render target (either single or multi textures).
 */
class BABYLON_SHARED_EXPORT RenderTargetWrapper {

public:
  using RenderTargetTextureSize = std::variant<int, RenderTargetSize, float>;

public:
  /**
   * @brief Initializes the render target wrapper.
   * @param isMulti true if the wrapper is a multi render target
   * @param isCube true if the wrapper should render to a cube texture
   * @param size size of the render target (width/height/layers)
   * @param engine engine used to create the render target
   */
  RenderTargetWrapper(bool isMulti, bool isCube, const RenderTargetTextureSize& size,
                      ThinEngine* engine);
  ~RenderTargetWrapper() = default;

protected:
  /**
   * @brief Gets the render texture. If this is a multi render target, gets the first texture
   */
  InternalTexturePtr& get_texture();

  /**
   * @brief Gets the list of render textures. If we are not in a multi render target, the list will
   * be null (use the texture getter instead)
   */
  std::vector<InternalTexturePtr>& get_textures();

  /**
   * @brief Gets the sample count of the render target
   */
  uint32_t get_samples() const;

public:
  /** @hidden */
  std::vector<uint32_t> _attachments;
  /** @hidden */
  bool _generateStencilBuffer;
  /** @hidden */
  bool _generateDepthBuffer;

  /** @hidden */
  InternalTexturePtr _depthStencilTexture;
  /** @hidden */
  bool _depthStencilTextureWithStencil;

  /**
   * Gets the render texture. If this is a multi render target, gets the first texture
   */
  ReadOnlyProperty<RenderTargetWrapper, InternalTexturePtr> texture;

  /**
   * Gets the list of render textures. If we are not in a multi render target, the list will be null
   * (use the texture getter instead)
   */
  ReadOnlyProperty<RenderTargetWrapper, std::vector<InternalTexturePtr>> textures;

  /**
   * Gets the sample count of the render target
   */
  ReadOnlyProperty<RenderTargetWrapper, uint32_t> samples;

protected:
  ThinEngine* _engine;

private:
  RenderTargetTextureSize _size;
  bool _isCube;
  bool _isMulti;
  std::vector<InternalTexturePtr> _textures;
  InternalTexturePtr _nullTexture;

}; // end of class RenderTargetWrapper

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINES_RENDER_TARGET_WRAPPER_H
