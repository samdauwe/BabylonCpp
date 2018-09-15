#ifndef BABYLON_INTERFACES_IRENDERABLE_SCENE_H
#define BABYLON_INTERFACES_IRENDERABLE_SCENE_H

#include <functional>
#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Engine;
class ICanvas;
class Scene;

class BABYLON_SHARED_EXPORT IRenderableScene {

public:
  IRenderableScene(ICanvas* canvas);
  virtual ~IRenderableScene();

  void initialize();

  virtual void render();
  virtual const char* getName()                               = 0;
  virtual void initializeScene(ICanvas* canvas, Scene* scene) = 0;

  Scene* getScene() const;
  Engine* getEngine() const;

protected:
  std::function<void()> _renderFunction;

protected:
  bool _initialized;
  ICanvas* _canvas;
  std::unique_ptr<Engine> _engine;
  std::unique_ptr<Scene> _scene;

}; // end of class IRenderableScene

} // end of namespace BABYLON

#endif // BABYLON_INTERFACES_IRENDERABLE_SCENE_H
