#ifndef BABYLON_SPRITES_SPRITE_MANAGER_H
#define BABYLON_SPRITES_SPRITE_MANAGER_H

#include <babylon/babylon_global.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/materials/textures/texture_constants.h>
#include <babylon/sprites/sprite.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT SpriteManager : public IDisposable {

public:
  template <typename... Ts>
  static SpriteManager* New(Ts&&... args)
  {
    auto spriteManager = new SpriteManager(std::forward<Ts>(args)...);
    spriteManager->addToScene(
      static_cast<std::unique_ptr<SpriteManager>>(spriteManager));

    return spriteManager;
  }
  virtual ~SpriteManager();

  void addToScene(std::unique_ptr<SpriteManager>&& newSpriteManager);

  Texture* texture() const;
  void texture(Texture* value);
  void setOnDispose(const ::std::function<void()>& callback);
  PickingInfo* intersects(const Ray ray, Camera* camera,
                          ::std::function<bool(Sprite* sprite)> predicate,
                          bool fastCheck);
  void render();
  void dispose(bool doNotRecurse = false) override;

protected:
  SpriteManager(const string_t& name, const string_t& imgUrl,
                unsigned int capacity, const ISize& cellSize, Scene* scene,
                float epsilon = 0.01f,
                unsigned int samplingMode
                = TextureConstants::TRILINEAR_SAMPLINGMODE);

private:
  void _appendSpriteVertex(size_t index, Sprite* sprite, int offsetX,
                           int offsetY, int rowSize);

public:
  string_t name;
  vector_t<std::unique_ptr<Sprite>> sprites;
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

private:
  Observer<SpriteManager>::Ptr _onDisposeObserver;
  size_t _capacity;
  Texture* _spriteTexture;
  Scene* _scene;
  Float32Array _vertexData;
  std::unique_ptr<Buffer> _buffer;
  std::unordered_map<string_t, std::unique_ptr<VertexBuffer>> _vertexBuffers;
  std::unordered_map<string_t, VertexBuffer*> _vertexBufferPtrs;
  std::unique_ptr<GL::IGLBuffer> _indexBuffer;
  Effect* _effectBase;
  Effect* _effectFog;

}; // end of class Sprite

} // end of namespace BABYLON

#endif // end of BABYLON_SPRITES_SPRITE_MANAGER_H
