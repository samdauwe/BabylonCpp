#ifndef BABYLON_SPRITES_SPRITE_MANAGER_H
#define BABYLON_SPRITES_SPRITE_MANAGER_H

#include <babylon/babylon_api.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/sprites/isprite_manager.h>
#include <babylon/sprites/sprite.h>
#include <babylon/misc/observable.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

class Buffer;
class Camera;
class Effect;
class PickingInfo;
class Ray;
class SpriteManager;
class Texture;
class VertexBuffer;
using CameraPtr        = std::shared_ptr<Camera>;
using EffectPtr        = std::shared_ptr<Effect>;
using SpriteManagerPtr = std::shared_ptr<SpriteManager>;
using TexturePtr       = std::shared_ptr<Texture>;
using VertexBufferPtr  = std::shared_ptr<VertexBuffer>;

namespace GL {
class IGLBuffer;
} // end of namespace GL

/**
 * @brief Class used to manage multiple sprites on the same spritesheet.
 * @see http://doc.babylonjs.com/babylon101/sprites
 */
class BABYLON_SHARED_EXPORT SpriteManager : public ISpriteManager {

public:
  template <typename... Ts>
  static SpriteManagerPtr New(Ts&&... args)
  {
    auto spriteManager = std::shared_ptr<SpriteManager>(
      new SpriteManager(std::forward<Ts>(args)...));
    spriteManager->addToScene(spriteManager);

    return spriteManager;
  }
  virtual ~SpriteManager();

  void addToScene(const SpriteManagerPtr& newSpriteManager);

  /**
   * @brief Intersects the sprites with a ray.
   * @param ray defines the ray to intersect with
   * @param camera defines the current active camera
   * @param predicate defines a predicate used to select candidate sprites
   * @param fastCheck defines if a fast check only must be done (the first
   * potential sprite is will be used and not the closer)
   * @returns null if no hit or a PickingInfo
   */
  std::optional<PickingInfo>
  intersects(const Ray ray, const CameraPtr& camera,
             std::function<bool(Sprite* sprite)> predicate,
             bool fastCheck) override;

  /**
   * @brief Render all child sprites.
   */
  void render() override;

  /**
   * @brief Release associated resources.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

protected:
  /**
   * @brief Creates a new sprite manager.
   * @param name defines the manager's name
   * @param imgUrl defines the sprite sheet url
   * @param capacity defines the maximum allowed number of sprites
   * @param cellSize defines the size of a sprite cell
   * @param scene defines the hosting scene
   * @param epsilon defines the epsilon value to align texture (0.01 by default)
   * @param samplingMode defines the smapling mode to use with spritesheet
   */
  SpriteManager(const std::string& name, const std::string& imgUrl,
                unsigned int capacity, const ISize& cellSize, Scene* scene,
                float epsilon = 0.01f,
                unsigned int samplingMode
                = TextureConstants::TRILINEAR_SAMPLINGMODE);

  /**
   * @brief Sets the callback called when the manager is disposed.
   */
  void set_onDispose(
    const std::function<void(SpriteManager*, EventState&)>& callback);

  /**
   * @brief Gets the spritesheet texture.
   */
  TexturePtr& get_texture();

  /**
   * @brief Sets the spritesheet texture.
   */
  void set_texture(const TexturePtr& value);

private:
  void _appendSpriteVertex(size_t index, const Sprite& sprite, int offsetX,
                           int offsetY, int rowSize);

public:
  /**
   * Defines the manager's name
   */
  std::string name;

  /**
   * Gets or sets a boolean indicating if the manager must consider scene fog
   * when rendering
   */
  bool fogEnabled;

  /**
   * Defines the default width of a cell in the spritesheet
   */
  int cellWidth;

  /**
   * Defines the default height of a cell in the spritesheet
   */
  int cellHeight;

  /**
   * An event triggered when the manager is disposed.
   */
  Observable<SpriteManager> onDisposeObservable;

  /**
   * Callback called when the manager is disposed
   */
  WriteOnlyProperty<SpriteManager,
                    std::function<void(SpriteManager*, EventState&)>>
    onDispose;

  /**
   * Gets or sets the spritesheet texture
   */
  Property<SpriteManager, TexturePtr> texture;

private:
  Observer<SpriteManager>::Ptr _onDisposeObserver;
  size_t _capacity;
  TexturePtr _spriteTexture;
  float _epsilon;
  Scene* _scene;
  Float32Array _vertexData;
  std::unique_ptr<Buffer> _buffer;
  std::unordered_map<std::string, VertexBufferPtr> _vertexBuffers;
  std::unique_ptr<GL::IGLBuffer> _indexBuffer;
  EffectPtr _effectBase;
  EffectPtr _effectFog;

}; // end of class Sprite

} // end of namespace BABYLON

#endif // end of BABYLON_SPRITES_SPRITE_MANAGER_H
