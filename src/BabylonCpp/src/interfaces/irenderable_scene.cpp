#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

IRenderableScene::IRenderableScene(ICanvas* canvas)
    : _renderFunction{nullptr}
    , _initialized{false}
    , _canvas{canvas}
    , _engine{nullptr}
    , _scene{nullptr}
{
}

IRenderableScene::~IRenderableScene()
{
}

void IRenderableScene::initialize(ICanvas* canvas)
{
  if (canvas && (canvas != _canvas))
  {
    _initialized = false;
    _canvas = canvas;
  }
  if (!_canvas)
    throw std::runtime_error("IRenderableScene::initialize without any canvas set");

  if (_initialized)
    return;

  // Load the 3D engine
  _engine = Engine::New(_canvas);
  // Creates the basic Babylon Scene object
  _scene = Scene::New(_engine.get());
  // Set the render function
  _renderFunction = [&]() { _scene->render(); };
  // Initialize the scene
  initializeScene(_canvas, _scene.get());
  _initialized = true;
}

void IRenderableScene::render()
{
  if (_renderFunction && _engine) {
    _engine->renderFunction(_renderFunction);
  }
}

Scene* IRenderableScene::getScene() const
{
  return _scene.get();
}

Engine* IRenderableScene::getEngine() const
{
  return _engine.get();
}

} // end of namespace BABYLON
