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
	: m_pCompilerLogger(0)
	, m_pRuntimeObjectSystem(0)
	, m_pUpdateable(0)
{
}

RtcManager::~RtcManager()
{
    if( m_pRuntimeObjectSystem )
    {
        // clean temp object files
        m_pRuntimeObjectSystem->CleanObjectFiles();
    }

    if( m_pRuntimeObjectSystem && m_pRuntimeObjectSystem->GetObjectFactorySystem() )
    {
        m_pRuntimeObjectSystem->GetObjectFactorySystem()->RemoveListener(this);

        // delete object via correct interface
        IObject* pObj = m_pRuntimeObjectSystem->GetObjectFactorySystem()->GetObject( m_ObjectId );
        delete pObj;
    }

	delete m_pRuntimeObjectSystem;
	delete m_pCompilerLogger;
}


bool RtcManager::Init()
{
	//Initialise the RuntimeObjectSystem
	m_pRuntimeObjectSystem = new RuntimeObjectSystem;
	m_pCompilerLogger = new LogSystem();
	if( !m_pRuntimeObjectSystem->Initialise(m_pCompilerLogger, 0) )
    {
        m_pRuntimeObjectSystem = 0;
        return false;
    }
	m_pRuntimeObjectSystem->GetObjectFactorySystem()->AddListener(this);

  SetIncludePath();
  SetCompileOptions();
  SetLibraryPath();

	// construct first object
	IObjectConstructor* pCtor = m_pRuntimeObjectSystem->GetObjectFactorySystem()->GetConstructor( "Sandbox" );
	if( pCtor )
	{
		IObject* pObj = pCtor->Construct();
		pObj->GetInterface( &m_pUpdateable );
		if( 0 == m_pUpdateable )
		{
			delete pObj;
			m_pCompilerLogger->LogError("Error - no updateable interface found\n");
			return false;
		}
		m_ObjectId = pObj->GetObjectId();

	}

	return true;
}

void RtcManager::SetCompileOptions()
{
#ifdef _MSC_VER
  m_pRuntimeObjectSystem->SetAdditionalCompileOptions("/std:c++17");
#endif // _MSC_VER
}


void RtcManager::SetLibraryPath()
{
  auto libPath = FileSystemUtils::GetExePath().ParentPath().ParentPath() / "lib";
  m_pRuntimeObjectSystem->AddLibraryDir(libPath.c_str());
}

void RtcManager::SetIncludePath()
{
  FileSystemUtils::Path basePath = m_pRuntimeObjectSystem->FindFile(__FILE__);
  FileSystemUtils::Path repoPath = basePath.ParentPath().ParentPath().ParentPath().ParentPath();
  FileSystemUtils::Path repoSrcPath = basePath.ParentPath().ParentPath().ParentPath().ParentPath() / "src";
  FileSystemUtils::Path repoExternalPath = basePath.ParentPath().ParentPath().ParentPath().ParentPath() / "external";
  m_pRuntimeObjectSystem->AddIncludeDir(repoSrcPath.c_str());
  m_pRuntimeObjectSystem->AddIncludeDir((repoExternalPath / "RuntimeCompiledCPlusPlus/Aurora").c_str());
  m_pRuntimeObjectSystem->AddIncludeDir((repoExternalPath / "imgui").c_str());
  m_pRuntimeObjectSystem->AddIncludeDir((repoExternalPath / "json.hpp").c_str());
  m_pRuntimeObjectSystem->AddIncludeDir((repoExternalPath / "glad/include").c_str());
  m_pRuntimeObjectSystem->AddIncludeDir((repoSrcPath / "BabylonCpp/include").c_str());
  m_pRuntimeObjectSystem->AddIncludeDir((repoSrcPath / "BabylonImGui/include").c_str());
  m_pRuntimeObjectSystem->AddIncludeDir((repoSrcPath / "MaterialsLibrary/include").c_str());
  m_pRuntimeObjectSystem->AddIncludeDir((repoSrcPath / "ProceduralTexturesLibrary/include").c_str());
  m_pRuntimeObjectSystem->AddIncludeDir((repoSrcPath / "Loaders/include").c_str());
  m_pRuntimeObjectSystem->AddIncludeDir((repoSrcPath / "imgui_utils").c_str());

  FileSystemUtils::Path apiPath = 
    FileSystemUtils::GetExePath().ParentPath().ParentPath().ParentPath() / "src/BabylonCpp/include";
  m_pRuntimeObjectSystem->AddIncludeDir(apiPath.c_str());
}



void RtcManager::OnConstructorsAdded()
{
	// This could have resulted in a change of object pointer, so release old and get new one.
	if( m_pUpdateable )
	{
		IObject* pObj = m_pRuntimeObjectSystem->GetObjectFactorySystem()->GetObject( m_ObjectId );
		pObj->GetInterface( &m_pUpdateable );
		if( 0 == m_pUpdateable )
		{
			delete pObj;
			m_pCompilerLogger->LogError( "Error - no updateable interface found\n");
		}
	}
}

void RtcManager::MainLoop()
{
  static int idx = 0;
  idx++;
  if (idx % 20 != 0)
    return;

	//check status of any compile
	if( m_pRuntimeObjectSystem->GetIsCompiledComplete() )
	{
		// load module when compile complete
		m_pRuntimeObjectSystem->LoadCompiledModule();
	}

	if( !m_pRuntimeObjectSystem->GetIsCompiling() )
	{
    static int numUpdates = 0;
		std::cout << "\nMain Loop. Updates every second. Update: " << numUpdates++ << "\n";
		const float deltaTime = 1.0f;
		m_pRuntimeObjectSystem->GetFileChangeNotifier()->Update( deltaTime );
		m_pUpdateable->Update( deltaTime );
	}
}

} // namespace BABYLON
} // namespace rtc
