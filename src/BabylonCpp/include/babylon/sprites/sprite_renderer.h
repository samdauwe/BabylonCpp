#ifndef BABYLON_SPRITES_SPRITE_RENDERER_H
#define BABYLON_SPRITES_SPRITE_RENDERER_H

#include <functional>
#include <memory>
#include <unordered_map>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/babylon_fwd.h>

namespace BABYLON {

namespace GL {
class IGLVertexArrayObject;
} // end of namespace GL

class Buffer;
struct ISize;
class Matrix;
class Scene;
class ThinEngine;
FWD_CLASS_SPTR(Effect)
FWD_CLASS_SPTR(ThinSprite)
FWD_CLASS_SPTR(ThinTexture)
FWD_CLASS_SPTR(VertexBuffer)
FWD_CLASS_SPTR(WebGLDataBuffer)
using WebGLVertexArrayObjectPtr = std::shared_ptr<GL::IGLVertexArrayObject>;

/**
 * @brief Class used to render sprites.
 *
 * It can be used either to render Sprites or ThinSriptes with ThinEngine only.
 */
class BABYLON_SHARED_EXPORT SpriteRenderer {

public:
  /**
   * @brief Creates a new sprite Renderer.
   * @param engine defines the engine the renderer works with
   * @param capacity defines the maximum allowed number of sprites
   * @param epsilon defines the epsilon value to align texture (0.01 by default)
   * @param scene defines the hosting scene
   */
  SpriteRenderer(ThinEngine* engine, size_t capacity, float epsilon = 0.01f,
                 Scene* scene = nullptr);
  ~SpriteRenderer(); // = default

  /**
   * @brief Render all child sprites.
   * @param sprites defines the list of sprites to render
   * @param deltaTime defines the time since last frame
   * @param viewMatrix defines the viewMatrix to use to render the sprites
   * @param projectionMatrix defines the projectionMatrix to use to render the sprites
   * @param customSpriteUpdate defines a custom function to update the sprites data before they
   * render
   */
  void
  render(const std::vector<ThinSpritePtr>& sprites, float deltaTime, const Matrix& viewMatrix,
         const Matrix& projectionMatrix,
         const std::function<void(ThinSprite* sprite, const ISize& baseSize)>& customSpriteUpdate
         = nullptr);

  /**
   * @brief Release associated resources.
   */
  void dispose();

protected:
  /**
   * @brief Gets the capacity of the manager
   */
  size_t get_capacity() const;

private:
  void _appendSpriteVertex(
    size_t index, const ThinSpritePtr& sprite, int offsetX, int offsetY, const ISize& baseSize,
    bool useRightHandedSystem,
    const std::function<void(ThinSprite* sprite, const ISize& baseSize)>& customSpriteUpdate);

public:
  /**
   * Defines the texture of the spritesheet
   */
  ThinTexturePtr texture;

  /**
   * Defines the default width of a cell in the spritesheet
   */
  int cellWidth;

  /**
   * Defines the default height of a cell in the spritesheet
   */
  int cellHeight;

  /**
   * Blend mode use to render the particle, it can be any of
   * the static Constants.ALPHA_x properties provided in this class.
   * Default value is Constants.ALPHA_COMBINE
   */
  unsigned int blendMode;

  /**
   * Gets or sets a boolean indicating if alpha mode is automatically
   * reset.
   */
  bool autoResetAlpha;

  /**
   * Disables writing to the depth buffer when rendering the sprites.
   * It can be handy to disable depth writing when using textures without alpha channel
   * and setting some specific blend modes.
   */
  bool disableDepthWrite;

  /**
   * Gets or sets a boolean indicating if the manager must consider scene fog when rendering
   */
  bool fogEnabled;

  /**
   * Gets the capacity of the manager
   */
  ReadOnlyProperty<SpriteRenderer, size_t> capacity;

private:
  ThinEngine* _engine;
  bool _useVAO;
  bool _useInstancing;
  Scene* _scene;

  size_t _capacity;
  float _epsilon;

  size_t _vertexBufferSize;
  Float32Array _vertexData;
  std::unique_ptr<Buffer> _buffer;
  std::unordered_map<std::string, VertexBufferPtr> _vertexBuffers;
  std::unique_ptr<Buffer> _spriteBuffer;
  WebGLDataBufferPtr _indexBuffer;
  EffectPtr _effectBase;
  EffectPtr _effectFog;
  WebGLVertexArrayObjectPtr _vertexArrayObject;

}; // end of class SpriteRenderer

} // end of namespace BABYLON

#endif // end of BABYLON_SPRITES_SPRITE_RENDERER_H
