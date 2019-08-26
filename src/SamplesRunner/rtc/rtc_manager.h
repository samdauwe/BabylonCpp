// Adapted from RuntimeCompiledCPlusPlus/Aurora/Examples/ConsoleExample/ConsoleGame.cpp
#pragma once

#include <RuntimeObjectSystem/IObjectFactorySystem.h>
#include <RuntimeObjectSystem/ObjectInterface.h>
#include <RuntimeCompiler/AUArray.h>

#ifndef _WIN32
int _getche();
#endif


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
	void MainLoop();

	// IObjectFactoryListener
  virtual void OnConstructorsAdded();
	// ~IObjectFactoryListener

private:
  void SetIncludePath();
  void SetCompileOptions();
  void SetLibraryPath();

	// Runtime Systems
	ICompilerLogger*		m_pCompilerLogger;
	IRuntimeObjectSystem*	m_pRuntimeObjectSystem;

	// Runtime object
	ISceneProducer* 			m_pUpdateable;
	ObjectId	   			m_ObjectId;
};

} // namespace BABYLON
} // namespace rtc
