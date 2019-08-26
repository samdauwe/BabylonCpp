#pragma once
#include <memory>
#include <SamplesRunner/rtc/suppress_warnings.h>
#include <RuntimeObjectSystem/IObject.h>
#include <babylon/interfaces/irenderable_scene.h>

namespace BABYLON {

namespace rtc {

struct ISceneProducer : public IObject
{
	virtual void Update( float deltaTime ) {}
  virtual std::shared_ptr<BABYLON::IRenderableScene> MakeScene() = 0;
};

} // namespace BABYLON
} // namespace rtc

