#ifndef BABYLON_LAYER_LAYER_H
#define BABYLON_LAYER_LAYER_H

#include <babylon/babylon_global.h>
#include <babylon/engine/scene.h>
#include <babylon/interfaces/idisposable.h>
#include <babylon/math/color4.h>
#include <babylon/math/vector2.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT Layer : public IDisposable {

public:
  Layer(const std::string& name, const std::string& imgUrl, Scene* scene,
        bool isBackground   = true,
        const Color4& color = Color4(1.f, 1.f, 1.f, 1.f));
  ~Layer();

  // Events
  void setOnDispose(const std::function<void()>& callback);
  void setOnBeforeRender(const std::function<void()>& callback);
  void setOnAfterRender(const std::function<void()>& callback);

  void render();
  void dispose(bool doNotRecurse = false) override;

public:
  // Events
  /**
   * An event triggered when the layer is disposed.
   * @type {BABYLON.Observable}
   */
  Observable<Layer> onDisposeObservable;
  /**
   * An event triggered before rendering the scene
   * @type {BABYLON.Observable}
   */
  Observable<Layer> onBeforeRenderObservable;
  /**
   * An event triggered after rendering the scene
   * @type {BABYLON.Observable}
   */
  Observable<Layer> onAfterRenderObservable;
  // Properties
  Texture* texture;
  bool isBackground;
  Color4 color;
  Vector2 scale;
  Vector2 offset;
  int alphaBlendingMode;
  bool alphaTest;

private:
  // Events
  Observer<Layer>::Ptr _onDisposeObserver;
  Observer<Layer>::Ptr _onBeforeRenderObserver;
  Observer<Layer>::Ptr _onAfterRenderObserver;
  // Properties
  std::string _name;
  Scene* _scene;
  std::unordered_map<std::string, VertexBuffer*> _vertexBuffers;
  std::unique_ptr<GL::IGLBuffer> _indexBuffer;
  Effect* _effect;
  Effect* _alphaTestEffect;

}; // end of class Layer

} // end of namespace BABYLON

#endif // end of BABYLON_LAYER_LAYER_H
