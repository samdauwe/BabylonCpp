// Adapted from RuntimeCompiledCPlusPlus/Aurora/Examples/ConsoleExample/ConsoleGame.cpp
#pragma once

#include <RuntimeObjectSystem/IObjectFactorySystem.h>
#include <RuntimeObjectSystem/ObjectInterface.h>
#include <RuntimeCompiler/AUArray.h>

#ifndef _WIN32
int _getche();
#endif


struct IUpdateable;
struct IRuntimeObjectSystem;

class RuntimeCompileWrapper : public IObjectFactoryListener
{
public:
	RuntimeCompileWrapper();
	virtual ~RuntimeCompileWrapper();

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
	IUpdateable* 			m_pUpdateable;
	ObjectId	   			m_ObjectId;
};
