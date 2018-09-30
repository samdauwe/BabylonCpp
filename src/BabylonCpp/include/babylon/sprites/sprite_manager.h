#ifndef BABYLON_SPRITES_SPRITE_MANAGER_H
#define BABYLON_SPRITES_SPRITE_MANAGER_H

#include <babylon/babylon_api.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/sprites/sprite.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

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
using SpriteManagerPtr = std::shared_ptr<SpriteManager>;
using TexturePtr       = std::shared_ptr<Texture>;

namespace GL {
class IGLBuffer;
} // end of namespace GL

class BABYLON_SHARED_EXPORT SpriteManager {

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

  void setOnDispose(
    const std::function<void(SpriteManager*, EventState&)>& callback);
  std::optional<PickingInfo>
  intersects(const Ray ray, const CameraPtr& camera,
             std::function<bool(Sprite* sprite)> predicate, bool fastCheck);
  void render();
  void dispose();

protected:
  SpriteManager(const std::string& name, const std::string& imgUrl,
                unsigned int capacity, const ISize& cellSize, Scene* scene,
                float epsilon = 0.01f,
                unsigned int samplingMode
                = TextureConstants::TRILINEAR_SAMPLINGMODE);

  TexturePtr& get_texture();
  void set_texture(const TexturePtr& value);

private:
  void _appendSpriteVertex(size_t index, const Sprite& sprite, int offsetX,
                           int offsetY, float rowSize);

public:
  std::string name;
  std::vector<SpritePtr> sprites;
  unsigned int renderingGroupId;
  unsigned int layerMask;
  bool fogEnabled;
  bool isPickable;
  float _epsilon;
  int cellWidth;
  int cellHeight;

  /**
   * An event triggered when the manager is disposed.
   */
  Observable<SpriteManager> onDisposeObservable;

  Property<SpriteManager, TexturePtr> texture;

private:
  Observer<SpriteManager>::Ptr _onDisposeObserver;
  size_t _capacity;
  TexturePtr _spriteTexture;
  Scene* _scene;
  Float32Array _vertexData;
  std::unique_ptr<Buffer> _buffer;
  std::unordered_map<std::string, std::unique_ptr<VertexBuffer>> _vertexBuffers;
  std::unordered_map<std::string, VertexBuffer*> _vertexBufferPtrs;
  std::unique_ptr<GL::IGLBuffer> _indexBuffer;
  Effect* _effectBase;
  Effect* _effectFog;

}; // end of class Sprite

} // end of namespace BABYLON

#endif // end of BABYLON_SPRITES_SPRITE_MANAGER_H
