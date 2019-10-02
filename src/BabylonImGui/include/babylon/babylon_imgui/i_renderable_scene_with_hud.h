#pragma once
#include <functional>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON
{
  class IRenderableSceneWithHud : public IRenderableScene
  {
  public:
    IRenderableSceneWithHud(ICanvas* iCanvas = nullptr);
    virtual ~IRenderableSceneWithHud() = default;

    std::function<void(void)> hudGui = {};
  };
}
