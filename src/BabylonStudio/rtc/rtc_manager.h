// Adapted from RuntimeCompiledCPlusPlus/Aurora/Examples/ConsoleExample/ConsoleGame.cpp
#pragma once

#include <RuntimeObjectSystem/IObjectFactorySystem.h>
#include <RuntimeObjectSystem/ObjectInterface.h>
#include <RuntimeCompiler/AUArray.h>
#include <memory>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/babylon_imgui/babylon_studio.h>

struct IRuntimeObjectSystem;

namespace BABYLON {
namespace rtc {

struct ISceneProducer;

class RtcManager : public IObjectFactoryListener
{
public:
	RtcManager();
	virtual ~RtcManager();

	bool Init();
  BABYLON::PlaygroundCompilerStatus Heartbeat();

	// IObjectFactoryListener
  virtual void OnConstructorsAdded();
	// ~IObjectFactoryListener

private:
  void SetIncludePath();
  void SetCompileOptions();
  void SetLibraryPath();

	// Runtime Systems
	ICompilerLogger*		_compilerLogger;
	IRuntimeObjectSystem*	_runtimeObjectSystem;

	// Runtime object
	ISceneProducer* 			_sceneProducer;
	ObjectId	   			_objectId;
  std::shared_ptr<BABYLON::IRenderableScene> _lastCompiledScene;
};

} // namespace BABYLON
} // namespace rtc
