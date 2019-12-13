#ifndef IMGUI_UTILS_IMGUI_RUNNER_H
#define IMGUI_UTILS_IMGUI_RUNNER_H
#include <functional>
#include <imgui.h>
#include "imgui_runner/abstract_runner.h"
#include <memory>
#include <string>


namespace ImGuiUtils {
namespace ImGuiRunner {
// GuiFunction : any function that displays widgets using ImGui
// and returns true if exit is desired
using GuiFunctionWithExit = std::function<bool(void)>;

// PostInitFunction : any function that will be called once
// OpenGl, Glfw, Glad and ImGui are inited
using PostInitFunction = std::function<void(void)>;

inline void EmptyPostInitFunction()
{
}

enum class DefaultWindowTypeOption {
  ProvideFullScreenWindow,
  ProvideFullScreenDockSpace,
  NoDefaultWindow
};

struct AppWindowParams {
  bool FullScreen = false;

  bool WindowedFullScreen             = false;
  int WindowedFullScreen_HeightReduce = 64;

  int Width         = 1280;
  int Height        = 720;
  std::string Title = "My Window";
  ImVec4 ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

  DefaultWindowTypeOption DefaultWindowType;

  bool ShowMenuBar                       = false;
  bool ConfigWindowsMoveFromTitleBarOnly = true;
  bool LoadFontAwesome                   = true;

  std::function<void(ImGuiID /* fullDockSpace */)> InitialDockLayoutFunction;
};

using ImGuiRunnerFunctionType
  = std::function<void(GuiFunctionWithExit, AppWindowParams, PostInitFunction)>;

class RunnerBabylon {
public:
  RunnerBabylon(std::unique_ptr<ImGui::ImGuiRunner::AbstractRunner> abstractRunner,
                const AppWindowParams& appWindowParams, GuiFunctionWithExit guiFunctionWithExit,
                PostInitFunction postInitFunction = EmptyPostInitFunction);

  void Run();
  static void ResetDockLayout();

private:
  std::unique_ptr<ImGui::ImGuiRunner::AbstractRunner> mAbstractRunner;
  AppWindowParams mAppWindowParams;
  GuiFunctionWithExit mGuiFunctionWithExit;
  PostInitFunction mPostInitFunction;
};

} // namespace ImGuiRunner
} // namespace ImGuiUtils

#endif // #ifdef IMGUI_UTILS_IMGUI_RUNNER_H
