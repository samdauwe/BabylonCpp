#ifndef BABYLON_SPRITES_SPRITE_MANAGER_H
#define BABYLON_SPRITES_SPRITE_MANAGER_H

#include <babylon/babylon_global.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/sprites/sprite.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT SpriteManager {

public:
  template <typename... Ts>
  static SpriteManager* New(Ts&&... args)
  {
    auto spriteManager = new SpriteManager(std::forward<Ts>(args)...);
    spriteManager->addToScene(
      static_cast<unique_ptr_t<SpriteManager>>(spriteManager));

    return spriteManager;
  }
  virtual ~SpriteManager();

  void addToScene(unique_ptr_t<SpriteManager>&& newSpriteManager);

  void setOnDispose(
    const std::function<void(SpriteManager*, EventState&)>& callback);
  nullable_t<PickingInfo>
  intersects(const Ray ray, const CameraPtr& camera,
             std::function<bool(Sprite* sprite)> predicate, bool fastCheck);
  void render();
  void dispose();

protected:
  SpriteManager(const string_t& name, const string_t& imgUrl,
                unsigned int capacity, const ISize& cellSize, Scene* scene,
                float epsilon = 0.01f,
                unsigned int samplingMode
                = TextureConstants::TRILINEAR_SAMPLINGMODE);

  TexturePtr& get_texture();
  void set_texture(const TexturePtr& value);

private:
  void _appendSpriteVertex(size_t index, Sprite* sprite, int offsetX,
                           int offsetY, int rowSize);

public:
  string_t name;
  vector_t<unique_ptr_t<Sprite>> sprites;
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
  unique_ptr_t<Buffer> _buffer;
  unordered_map_t<string_t, unique_ptr_t<VertexBuffer>> _vertexBuffers;
  unordered_map_t<string_t, VertexBuffer*> _vertexBufferPtrs;
  unique_ptr_t<GL::IGLBuffer> _indexBuffer;
  Effect* _effectBase;
  Effect* _effectFog;

}; // end of class Sprite

} // end of namespace BABYLON

#endif // end of BABYLON_SPRITES_SPRITE_MANAGER_H
