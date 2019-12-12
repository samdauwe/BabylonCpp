#include "runner_babylon.h"

#include <glad/glad.h>
//#include <imgui_utils/app_runner/details/imgui_runner_utils.h>

namespace ImGuiUtils
{
namespace ImGuiRunner2
{
//void RunGui_WithExit( // type is ImGuiRunnerFunctionType
//  GuiFunctionWithExit guiFunction,
//  AppWindowParams appWindowParams,
//  PostInitFunction postInitFunction
//)
//{
//}
//void RunGui(
//  GuiFunction guiFunction,
//  AppWindowParams appWindowParams,
//  PostInitFunction postInitFunction
//)
//{
//  GuiFunctionWithExit guiFunctionWithExit = [&]() -> bool {
//    guiFunction();
//    return false;
//  };
//  RunGui_WithExit(guiFunctionWithExit, appWindowParams, postInitFunction);
//}

//void ResetDockLayout()
//{
//  //details::SetDockLayout_NotDone();
//}


#if defined(__APPLE__)
std::pair<int, int> MainScreenResolution()
{
  return { 1280, 720 };
}
#elif defined(__EMSCRIPTEN__)
std::pair<int, int> MainScreenResolution()
{
  return { 1280, 720 };
}
#elif defined(_WIN32)
#undef APIENTRY
  #include <windows.h>
  std::pair<int, int> MainScreenResolution()
  {
    return {
      (int)GetSystemMetrics(SM_CXSCREEN),
      (int)GetSystemMetrics(SM_CYSCREEN)
    };
  }
#elif defined(__linux__)
  #include <X11/Xlib.h>
  std::pair<int, int> MainScreenResolution()
  {
    Display* d = XOpenDisplay(NULL);
    Screen* s  = DefaultScreenOfDisplay(d);
    return { s->width, s->height };
  }
#endif


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
  mAbstractRunner->SetBackendFullScreen(appWindowParams.FullScreen);
  mAbstractRunner->SetBackendWindowTitle(appWindowParams.Title);
  mAbstractRunner->SetBackendWindowSize(ImVec2((float)appWindowParams.Width, (float)appWindowParams.Height) );

  if (appWindowParams.WindowedFullScreen)
  {
    ImVec2 winSize;
    winSize.x = (float)ImGui::ImGuiRunner::MainScreenResolution().x;
    winSize.y = (float)ImGui::ImGuiRunner::MainScreenResolution().y - appWindowParams.WindowedFullScreen_HeightReduce;
    mAbstractRunner->SetBackendWindowSize(winSize);
  }

  mAbstractRunner->SetClearColor(appWindowParams.ClearColor);

//  DefaultWindowTypeOption DefaultWindowType;
//
//  bool ShowMenuBar = false;
//  bool ConfigWindowsMoveFromTitleBarOnly = true;
//  bool LoadFontAwesome = true;

}
void RunnerBabylon::Run()
{
  mAbstractRunner->RunIt();
}

} // namespace ImGuiRunner
} // namespace ImGuiUtils
