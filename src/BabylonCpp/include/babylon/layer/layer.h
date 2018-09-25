#ifndef BABYLON_LAYER_LAYER_H
#define BABYLON_LAYER_LAYER_H

#include <babylon/babylon_api.h>
#include <babylon/engine/scene.h>
#include <babylon/math/color4.h>
#include <babylon/math/vector2.h>
#include <babylon/tools/observable.h>
#include <babylon/tools/observer.h>

namespace BABYLON {

class Layer;
class Texture;
class VertexBuffer;
using TexturePtr = std::shared_ptr<Texture>;

namespace GL {
class IGLBuffer;
} // end of namespace GL

class BABYLON_SHARED_EXPORT Layer
    : public std::enable_shared_from_this<Layer> {

  using LayerCallbackType = std::function<void(Layer*, EventState&)>;

public:
  template <typename... Ts>
  static LayerPtr New(Ts&&... args)
  {
    return std::shared_ptr<Layer>(new Layer(std::forward<Ts>(args)...));
  }
  virtual ~Layer();

  /** Hidden */
  void _rebuild();
  void render();
  void dispose();

protected:
  Layer(const std::string& name, const std::string& imgUrl, Scene* scene,
        bool isBackground   = true,
        const Color4& color = Color4(1.f, 1.f, 1.f, 1.f));

  // Events
  void set_onDispose(const LayerCallbackType& callback);
  void set_onBeforeRender(const LayerCallbackType& callback);
  void set_onAfterRender(const LayerCallbackType& callback);

private:
  void _createIndexBuffer();

public:
  // Events

  /**
   * An event triggered when the layer is disposed.
   */
  Observable<Layer> onDisposeObservable;

  /**
   * An event triggered before rendering the scene
   */
  Observable<Layer> onBeforeRenderObservable;

  /**
   * An event triggered after rendering the scene
   */
  Observable<Layer> onAfterRenderObservable;

  // Properties
  TexturePtr texture;
  bool isBackground;
  Color4 color;
  Vector2 scale;
  Vector2 offset;
  unsigned int alphaBlendingMode;
  bool alphaTest;
  unsigned int layerMask;

  WriteOnlyProperty<Layer, LayerCallbackType> onDispose;
  WriteOnlyProperty<Layer, LayerCallbackType> onBeforeRender;
  WriteOnlyProperty<Layer, LayerCallbackType> onAfterRender;

private:
  // Events
  Observer<Layer>::Ptr _onDisposeObserver;
  Observer<Layer>::Ptr _onBeforeRenderObserver;
  Observer<Layer>::Ptr _onAfterRenderObserver;
  // Properties
  std::string _name;
  Scene* _scene;
  std::unordered_map<std::string, std::unique_ptr<VertexBuffer>> _vertexBuffers;
  std::unique_ptr<GL::IGLBuffer> _indexBuffer;
  Effect* _effect;
  Effect* _alphaTestEffect;

}; // end of class Layer

} // end of namespace BABYLON

#endif // end of BABYLON_LAYER_LAYER_H
