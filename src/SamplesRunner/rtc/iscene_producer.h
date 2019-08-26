#pragma once
#include <SamplesRunner/rtc/suppress_warnings.h>
#include <RuntimeObjectSystem/IObject.h>
#include <memory>

namespace BABYLON {

class IRenderableScene;

namespace rtc {

struct ISceneProducer : public IObject
{
	virtual void Update( float deltaTime ) = 0;
  //virtual std::shared_ptr<BABYLON::IRenderableScene> MakeScene() = 0;
};

} // namespace BABYLON
} // namespace rtc

