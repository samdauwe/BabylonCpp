#include "runner_babylon.h"

#include <babylon/core/filesystem.h>
#include <babylon/babylon_common.h>
#include <glad/glad.h>
#include "imgui_utils/icons_font_awesome_5.h"
#include <imgui_utils/imgui_utils.h>
#include "imgui_runner/runner_glfw.h"
#include "imgui_runner/runner_sdl.h"
#include "imgui_runner/runner_emscripten.h"

namespace ImGuiUtils
{
namespace ImGuiRunner {
namespace runner_babylon_details
{
static ImFont* _fontRegular = nullptr;
static ImFont* _fontSolid = nullptr;

void LoadFontAwesome()
{
  // Loads fonts
  ImGuiIO& io = ImGui::GetIO();
  io.Fonts->AddFontDefault();
  static ImWchar ranges[] = { 0xf000, 0xf82f, 0 };
  ImFontConfig config;
  config.MergeMode = true;
  auto fontRegularPath
    = std::string(BABYLON::assets_folder() + "/fonts/") + FONT_ICON_FILE_NAME_FAR;
  if (BABYLON::Filesystem::isFile(fontRegularPath))
  {
    _fontRegular = io.Fonts->AddFontFromFileTTF(fontRegularPath.c_str(),
                                                ImGui::IconSize, &config, ranges);
  }
  else
  {
    printf("Could not read font file %s", fontRegularPath.c_str());
  }

  auto fontSolidPath
    = std::string("../assets/fonts/") + FONT_ICON_FILE_NAME_FAS;
  if (! BABYLON::Filesystem::isFile(fontSolidPath))
  {
    fontSolidPath
      = std::string("assets/fonts/") + FONT_ICON_FILE_NAME_FAS;
  }
  if (BABYLON::Filesystem::isFile(fontSolidPath))
  {
    _fontSolid = io.Fonts->AddFontFromFileTTF(fontSolidPath.c_str(),
                                              ImGui::IconSize, &config, ranges);
  }
  else
  {
    printf("Could not read font file %s", fontSolidPath.c_str());
  }

}


void ImplProvideFullScreenWindow(const AppWindowParams& appWindowParams)
{
  ImGui::SetNextWindowPos(ImVec2(0, 0));
  ImVec2 winSize = ImGui::GetIO().DisplaySize;
  // winSize.y -= 10.f;
  ImGui::SetNextWindowSize(winSize);
  ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration
                                 | ImGuiWindowFlags_NoScrollWithMouse
                                 | ImGuiWindowFlags_NoBringToFrontOnFocus;
  if (appWindowParams.ShowMenuBar)
    windowFlags |= ImGuiWindowFlags_MenuBar;
  ImGui::Begin("Main window (title bar invisible)", nullptr, windowFlags);
}

ImGuiID MainDockSpaceId()
{
  static ImGuiID id = ImGui::GetID("MainDockSpace");
  return id;
}

void ImplProvideFullScreenDockSpace(const AppWindowParams& appWindowParams)
{
  ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::SetNextWindowPos(viewport->Pos);
  ImGui::SetNextWindowSize(viewport->Size);
  ImGui::SetNextWindowViewport(viewport->ID);
  ImGui::SetNextWindowBgAlpha(0.0f);

  ImGuiWindowFlags window_flags
    = ImGuiWindowFlags_NoDocking;
  window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse
                  | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
  window_flags
    |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  if (appWindowParams.ShowMenuBar)
    window_flags |= ImGuiWindowFlags_MenuBar;

  ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  static bool p_open = true;
  ImGui::Begin(appWindowParams.Title.c_str(), &p_open, window_flags);
  ImGui::PopStyleVar(3);

  ImGuiID dockspace_id = MainDockSpaceId();
  ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_PassthruCentralNode; // ImGuiDockNodeFlags_PassthruDockspace;
  ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
}


bool gWasDockLayoutDone = false;

bool WasDockLayoutDone()
{
  return gWasDockLayoutDone;
}
void SetDockLayout_Done()
{
  gWasDockLayoutDone = true;
}
void SetDockLayout_NotDone()
{
  remove("imgui.ini");
  gWasDockLayoutDone = false;
}

} // namespace runner_babylon_details



RunnerBabylon::RunnerBabylon(
  std::unique_ptr<ImGui::ImGuiRunner::AbstractRunner> abstractRunner,
  const AppWindowParams& appWindowParams,
  GuiFunctionWithExit guiFunctionWithExit,
  PostInitFunction postInitFunction)
  : mAbstractRunner(std::move(abstractRunner))
  , mAppWindowParams(appWindowParams)
  , mGuiFunctionWithExit(guiFunctionWithExit)
  , mPostInitFunction(postInitFunction)
{
  DoInit();
}

void RunnerBabylon::DoInit()
{
  mAbstractRunner->SetBackendFullScreen(mAppWindowParams.FullScreen);
  mAbstractRunner->SetBackendWindowTitle(mAppWindowParams.Title);
  mAbstractRunner->SetBackendWindowSize(ImVec2((float)mAppWindowParams.Width, (float)mAppWindowParams.Height) );

  if (mAppWindowParams.WindowedFullScreen)
  {
    ImVec2 winSize;
    winSize.x = (float)ImGui::ImGuiRunner::MainScreenResolution().x;
    winSize.y = (float)ImGui::ImGuiRunner::MainScreenResolution().y - mAppWindowParams.WindowedFullScreen_HeightReduce;
    mAbstractRunner->SetBackendWindowSize(winSize);
  }

  mAbstractRunner->SetClearColor(mAppWindowParams.ClearColor);

  mAbstractRunner->LoadFonts = runner_babylon_details::LoadFontAwesome;

  auto postInit = [this]() {
    if (mAppWindowParams.DefaultWindowType == DefaultWindowTypeOption::ProvideFullScreenDockSpace)
      ImGui::GetIO().ConfigFlags = ImGui::GetIO().ConfigFlags | ImGuiConfigFlags_DockingEnable;

    mPostInitFunction();
    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = mAppWindowParams.ConfigWindowsMoveFromTitleBarOnly;

  };

  std::function<void(void)> provideWindowOrDock = [this](){
    if (mAppWindowParams.DefaultWindowType == DefaultWindowTypeOption::ProvideFullScreenWindow)
      runner_babylon_details::ImplProvideFullScreenWindow(mAppWindowParams);

    if (mAppWindowParams.DefaultWindowType == DefaultWindowTypeOption::ProvideFullScreenDockSpace)
    {
      if (!runner_babylon_details::WasDockLayoutDone())
      {
        if (mAppWindowParams.InitialDockLayoutFunction)
          mAppWindowParams.InitialDockLayoutFunction(runner_babylon_details::MainDockSpaceId());
        runner_babylon_details::SetDockLayout_Done();
      }
      runner_babylon_details::ImplProvideFullScreenDockSpace(mAppWindowParams);
    }
  };

  std::function<bool(void)> showGui = [this, postInit, provideWindowOrDock]() {
    static bool postInited = false;
    if (!postInited)
    {
      postInit();
      postInited = true;
    }
    provideWindowOrDock();
    bool shouldExit = mGuiFunctionWithExit();

    if (mAppWindowParams.DefaultWindowType != DefaultWindowTypeOption::NoDefaultWindow)
      ImGui::End();
    return shouldExit;
  };

  mAbstractRunner->ShowGui = showGui;
}

void RunnerBabylon::Run()
{
  mAbstractRunner->Run();
}

void RunnerBabylon::ResetDockLayout()
{
  runner_babylon_details::SetDockLayout_NotDone();
}

#ifdef IMGUI_RUNNER_USE_GLFW
void InvokeRunnerBabylonGlfw(const AppWindowParams& appWindowParams,
                             GuiFunctionWithExit guiFunctionWithExit,
                             PostInitFunction postInitFunction)
{
  auto runner = std::make_unique<ImGui::ImGuiRunner::RunnerGlfw>();
  auto runner_babylon = RunnerBabylon(
    std::move(runner),
    appWindowParams,
    guiFunctionWithExit,
    postInitFunction
    );
  runner_babylon.Run();
}
#endif

#ifdef IMGUI_RUNNER_USE_SDL
void InvokeRunnerBabylonSdl(const AppWindowParams& appWindowParams,
                             GuiFunctionWithExit guiFunctionWithExit,
                             PostInitFunction postInitFunction)
{
  auto runner = std::make_unique<ImGui::ImGuiRunner::RunnerSdl>();
  auto runner_babylon = RunnerBabylon(
    std::move(runner),
    appWindowParams,
    guiFunctionWithExit,
    postInitFunction
  );
  runner_babylon.Run();
}
#endif

#ifdef EMSCRIPTEN
void InvokeRunnerBabylonEmscripten(const AppWindowParams& appWindowParams,
                                   GuiFunctionWithExit guiFunctionWithExit,
                                   PostInitFunction postInitFunction)
{
  auto runner = std::make_unique<ImGui::ImGuiRunner::RunnerEmscripten>();
  auto runner_babylon = RunnerBabylon(
    std::move(runner),
    appWindowParams,
    guiFunctionWithExit,
    postInitFunction
  );
  runner_babylon.Run();
}
#endif

void InvokeRunnerBabylon(const AppWindowParams& appWindowParams,
                         GuiFunctionWithExit guiFunctionWithExit,
                         PostInitFunction postInitFunction)
{
#ifdef EMSCRIPTEN
  InvokeRunnerBabylonEmscripten(appWindowParams, guiFunctionWithExit, postInitFunction);
#elif defined(IMGUI_RUNNER_USE_GLFW)
  InvokeRunnerBabylonGlfw(appWindowParams, guiFunctionWithExit, postInitFunction);
#elif defined(IMGUI_RUNNER_USE_SDL)
  InvokeRunnerBabylonSdl(appWindowParams, guiFunctionWithExit, postInitFunction);
#else
  #error "InvokeRunnerBabylon: Not Backend available!"
#endif
}


} // namespace ImGuiRunner
} // namespace ImGuiUtils
