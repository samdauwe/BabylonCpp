#ifndef BABYLON_INTERFACES_IRENDERABLE_SCENE_WITH_HUD_H
#define BABYLON_INTERFACES_IRENDERABLE_SCENE_WITH_HUD_H

#include <functional>

#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT IRenderableSceneWithHud : public IRenderableScene {

public:
  IRenderableSceneWithHud(ICanvas* iCanvas = nullptr);
  ~IRenderableSceneWithHud() override = default;

  std::function<void(void)> hudGui = {};
};

} // end of namespace BABYLON

#endif // BABYLON_INTERFACES_IRENDERABLE_SCENE_WITH_HUD_H
