#ifdef BABYLON_BUILD_SANDBOX

// Adapted from RuntimeCompiledCPlusPlus/Aurora/Examples/ConsoleExample/ConsoleGame.cpp
#include <SamplesRunner/rtc/suppress_warnings.h>
#include <SamplesRunner/rtc/rtc_manager.h>
#include <SamplesRunner/rtc/log_system.h>

#include <RuntimeCompiler/AUArray.h>
#include <RuntimeCompiler/BuildTool.h>
#include <RuntimeCompiler/ICompilerLogger.h>
#include <RuntimeCompiler/FileChangeNotifier.h>
#include <RuntimeObjectSystem/IObjectFactorySystem.h>
#include <RuntimeObjectSystem/ObjectFactorySystem/ObjectFactorySystem.h>
#include <RuntimeObjectSystem/RuntimeObjectSystem.h>
#include <RuntimeObjectSystem/IObject.h>

#include <SamplesRunner/rtc/iscene_producer.h>
#include <SamplesRunner/rtc/interface_ids.h>
#include <babylon/core/system.h>
#include <babylon/core/filesystem.h>

#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <string>
#include <stdarg.h>

// Remove windows.h define of GetObject which conflicts with EntitySystem GetObject
#if defined _WINDOWS_ && defined GetObject
#undef GetObject
#endif
using FileSystemUtils::Path;

namespace BABYLON {
namespace rtc {

RtcManager::RtcManager()
	: _compilerLogger(0)
	, _runtimeObjectSystem(0)
	, _sceneProducer(0)
{
}

RtcManager::~RtcManager()
{
    if( _runtimeObjectSystem )
    {
        // clean temp object files
        _runtimeObjectSystem->CleanObjectFiles();
    }

    if( _runtimeObjectSystem && _runtimeObjectSystem->GetObjectFactorySystem() )
    {
        _runtimeObjectSystem->GetObjectFactorySystem()->RemoveListener(this);

        // delete object via correct interface
        IObject* pObj = _runtimeObjectSystem->GetObjectFactorySystem()->GetObject( _objectId );
        delete pObj;
    }

	delete _runtimeObjectSystem;
	delete _compilerLogger;
}


bool RtcManager::Init()
{
	//Initialise the RuntimeObjectSystem
	_runtimeObjectSystem = new RuntimeObjectSystem;
	_compilerLogger = new LogSystem();
	if( !_runtimeObjectSystem->Initialise(_compilerLogger, 0) )
    {
        _runtimeObjectSystem = 0;
        return false;
    }
	_runtimeObjectSystem->GetObjectFactorySystem()->AddListener(this);

  SetIncludePath();
  SetCompileOptions();
  SetLibraryPath();

	// construct first object
	IObjectConstructor* pCtor = _runtimeObjectSystem->GetObjectFactorySystem()->GetConstructor( "Sandbox" );
	if( pCtor )
	{
		IObject* pObj = pCtor->Construct();
		pObj->GetInterface( &_sceneProducer );
		if( 0 == _sceneProducer )
		{
			delete pObj;
			_compilerLogger->LogError("Error - no updateable interface found\n");
			return false;
		}
		_objectId = pObj->GetObjectId();

	}

	return true;
}

void RtcManager::SetCompileOptions()
{
#ifdef _MSC_VER
  _runtimeObjectSystem->SetAdditionalCompileOptions(
    " /std:c++17"
    " /DBABYLON_BUILD_SANDBOX"
    #ifdef NDEBUG
      " /DNDEBUG"
      " /MD"
    #else
      " /MDd"
    #endif
  );
#else
  _runtimeObjectSystem->SetAdditionalCompileOptions(
    " -std=c++17"
    " -DBABYLON_BUILD_SANDBOX"
    #ifdef NDEBUG
      " -DNDEBUG"
    #endif
  );
#endif // _MSC_VER
}


void RtcManager::SetLibraryPath()
{
  std::string exePath = BABYLON::System::getExecutablePath();
  std::string exeFolder = BABYLON::Filesystem::baseDir(exePath);

  std::string libPath = BABYLON::Filesystem::absolutePath(exeFolder + "/../lib");
  _runtimeObjectSystem->AddLibraryDir(libPath.c_str());
}

void RtcManager::SetIncludePath()
{
  FileSystemUtils::Path basePath = _runtimeObjectSystem->FindFile(__FILE__);
  FileSystemUtils::Path repoPath = basePath.ParentPath().ParentPath().ParentPath().ParentPath();
  FileSystemUtils::Path repoSrcPath = basePath.ParentPath().ParentPath().ParentPath().ParentPath() / "src";
  FileSystemUtils::Path repoExternalPath = basePath.ParentPath().ParentPath().ParentPath().ParentPath() / "external";
  _runtimeObjectSystem->AddIncludeDir(repoSrcPath.c_str());
  _runtimeObjectSystem->AddIncludeDir((repoExternalPath / "RuntimeCompiledCPlusPlus/Aurora").c_str());
  _runtimeObjectSystem->AddIncludeDir((repoExternalPath / "imgui").c_str());
  _runtimeObjectSystem->AddIncludeDir((repoExternalPath / "json.hpp").c_str());
  _runtimeObjectSystem->AddIncludeDir((repoExternalPath / "glad/include").c_str());
  _runtimeObjectSystem->AddIncludeDir((repoSrcPath / "BabylonCpp/include").c_str());
  _runtimeObjectSystem->AddIncludeDir((repoSrcPath / "BabylonImGui/include").c_str());
  _runtimeObjectSystem->AddIncludeDir((repoSrcPath / "MaterialsLibrary/include").c_str());
  _runtimeObjectSystem->AddIncludeDir((repoSrcPath / "ProceduralTexturesLibrary/include").c_str());
  _runtimeObjectSystem->AddIncludeDir((repoSrcPath / "Loaders/include").c_str());
  _runtimeObjectSystem->AddIncludeDir((repoSrcPath / "imgui_utils").c_str());

  std::string exePath = BABYLON::System::getExecutablePath();
  std::string exeFolder = BABYLON::Filesystem::baseDir(exePath);
  std::string apiPath = BABYLON::Filesystem::absolutePath(exeFolder + "/../../src/BabylonCpp/include");
  _runtimeObjectSystem->AddIncludeDir(apiPath.c_str());
}



void RtcManager::OnConstructorsAdded()
{
	// This could have resulted in a change of object pointer, so release old and get new one.
	if( _sceneProducer )
	{
		IObject* pObj = _runtimeObjectSystem->GetObjectFactorySystem()->GetObject( _objectId );
		pObj->GetInterface( &_sceneProducer );
    _lastCompiledScene = _sceneProducer->MakeScene();
		if( 0 == _sceneProducer )
		{
			delete pObj;
			_compilerLogger->LogError( "Error - no updateable interface found\n");
		}
	}
}

BABYLON::SandboxCompilerStatus RtcManager::Heartbeat()
{
	//check status of any compile
	if( _runtimeObjectSystem->GetIsCompiledComplete() )
	{
		// load module when compile complete
		_runtimeObjectSystem->LoadCompiledModule();
	}

	if(_runtimeObjectSystem->GetIsCompiling() )
	{
    BABYLON::SandboxCompilerStatus r;
    r._isCompiling = true;
    return r;
	}
  else
  {
    const float deltaTime = 1.0f;
    _runtimeObjectSystem->GetFileChangeNotifier()->Update(deltaTime);
    _sceneProducer->Update(deltaTime);
  }

  if (_lastCompiledScene)
  {
    BABYLON::SandboxCompilerStatus r;
    r._renderableScene = _lastCompiledScene;
    _lastCompiledScene.reset();
    return r;
  }
  return BABYLON::SandboxCompilerStatus();
}

} // namespace BABYLON
} // namespace rtc

#endif // BABYLON_BUILD_SANDBOX
