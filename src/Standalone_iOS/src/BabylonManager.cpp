#include "BabylonManager.h"
#include "basic_canvas.h"
#include "HelloScene.h"

// Babylon
#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>

BabylonManager::BabylonManager(int width, int height):
  _width(width),
  _height(height),
  _renderableScene(nullptr)
{
  _renderCanvas = std::make_unique<BasicCanvas>();
  _renderCanvas->clientWidth = _width;
  _renderCanvas->clientHeight = _height;
  _renderCanvas->setFrameSize(_width, _height);
  
  _renderableScene = Samples::MakeHelloScene(_renderCanvas.get());
  _renderableScene->initialize(_renderCanvas.get());
}

BabylonManager::~BabylonManager()
{
}

void BabylonManager::render()
{
  // Render Scene
  _renderableScene->render();
}

void BabylonManager::setSize(int width, int height)
{
  if(_width != width || _height != height)
  {
    _width = width;
    _height = height;
    
    _renderCanvas->setFrameSize(_width, _height);
    _renderableScene->getEngine()->resize();
  }
}
