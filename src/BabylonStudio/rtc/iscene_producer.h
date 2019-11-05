#pragma once
#include <RuntimeObjectSystem/IObject.h>
#include <BabylonStudio/rtc/suppress_warnings.h>
#include <babylon/interfaces/irenderable_scene_with_hud.h>
#include <memory>

namespace BABYLON {

namespace rtc {

struct ISceneProducer : public IObject {
  virtual void Update(float deltaTime)
  {
    (void)deltaTime;
  }
  virtual std::shared_ptr<BABYLON::IRenderableSceneWithHud> MakeScene() = 0;
};

} // namespace rtc
} // namespace BABYLON
