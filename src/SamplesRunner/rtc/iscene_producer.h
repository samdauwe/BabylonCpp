#pragma once
#include <memory>
#include <SamplesRunner/rtc/suppress_warnings.h>
#include <RuntimeObjectSystem/IObject.h>
#include <babylon/babylon_imgui/i_renderable_scene_with_hud.h>

namespace BABYLON {

namespace rtc {

struct ISceneProducer : public IObject
{
	virtual void Update( float deltaTime ) { (void) deltaTime; }
  virtual std::shared_ptr<BABYLON::IRenderableSceneWithHud> MakeScene() = 0;
};

} // namespace BABYLON
} // namespace rtc

