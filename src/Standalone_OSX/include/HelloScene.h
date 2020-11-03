#pragma once
#include <memory>
#include <babylon/interfaces/icanvas.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace Samples {

// This files demonstrates how to create a very simple renderable scene
std::shared_ptr<BABYLON::IRenderableScene> MakeHelloScene(BABYLON::ICanvas* canvas);
} // end of namespace Samples
