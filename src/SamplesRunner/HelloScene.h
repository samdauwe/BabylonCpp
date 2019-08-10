#pragma once

#include <babylon/interfaces/irenderable_scene.h>

namespace SamplesRunner 
{
  struct HelloScene : public BABYLON::IRenderableScene
  {
    HelloScene(BABYLON::ICanvas* iCanvas);
    ~HelloScene() override;

    const char* getName() override;
    void initializeScene(BABYLON::ICanvas* canvas, BABYLON::Scene* scene) override;

  }; // end of struct BasicScene
}


