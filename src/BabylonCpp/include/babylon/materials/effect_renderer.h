#ifndef BABYLON_MATERIALS_EFFECT_RENDERER_H
#define BABYLON_MATERIALS_EFFECT_RENDERER_H

#include <memory>
#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

class Effect;
struct EffectWrapper;
class Engine;
class Texture;
class VertexBuffer;
class Viewport;
struct IEffectRendererOptions;
using EffectPtr        = std::shared_ptr<Effect>;
using EffectWrapperPtr = std::shared_ptr<EffectWrapper>;
using TexturePtr       = std::shared_ptr<Texture>;
using VertexBufferPtr  = std::shared_ptr<VertexBuffer>;

namespace GL {
class IGLBuffer;
} // end of namespace GL

/**
 * @brief Helper class to render one or more effects.
 */
class BABYLON_SHARED_EXPORT EffectRenderer {

private:
  // Fullscreen quad buffers by default.
  static IEffectRendererOptions _DefaultOptions;

public:
  /**
   * @brief Creates an effect renderer
   * @param engine the engine to use for rendering
   * @param options defines the options of the effect renderer
   */
  EffectRenderer(Engine* engine,
                 const IEffectRendererOptions& options = EffectRenderer::_DefaultOptions);
  ~EffectRenderer();

  /**
   * @brief Sets the current viewport in normalized coordinates 0-1.
   */
  void setViewport();

  /**
   * @brief Sets the current viewport in normalized coordinates 0-1.
   * @param viewport Defines the viewport to set (defaults to 0 0 1 1)
   */
  void setViewport(Viewport& viewport);

  /**
   * @brief Binds the embedded attributes buffer to the effect.
   * @param effect Defines the effect to bind the attributes for
   */
  void bindBuffers(const EffectPtr& effect);

  /**
   * @brief Sets the current effect wrapper to use during draw.
   * The effect needs to be ready before calling this api.
   * This also sets the default full screen position attribute.
   * @param effectWrapper Defines the effect to draw with
   */
  void applyEffectWrapper(const EffectWrapperPtr& effectWrapper);

  /**
   * @brief Draws a full screen quad.
   */
  void draw();

  /**
   * @brief Renders one or more effects to a specified texture.
   * @param effectWrappers list of effects to renderer
   * @param outputTexture texture to draw to, if null it will render to the screen
   */
  void render(const std::vector<EffectWrapperPtr>& effectWrappers,
              const TexturePtr& outputTexture = nullptr);

  /**
   * @brief Disposes of the effect renderer.
   */
  void dispose();

private:
  TexturePtr _getNextFrameBuffer(bool incrementIndex = true);

public:
  Engine* engine;

private:
  std::unordered_map<std::string, VertexBufferPtr> _vertexBuffers;
  std::unique_ptr<GL::IGLBuffer> _indexBuffer;

  size_t _ringBufferIndex;
  std::vector<TexturePtr> _ringScreenBuffer;
  std::unique_ptr<Viewport> _fullscreenViewport;

}; // end of class EffectRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_MATERIALS_EFFECT_RENDERER_H
