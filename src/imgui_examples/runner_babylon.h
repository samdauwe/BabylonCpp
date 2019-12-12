#ifndef IMGUI_UTILS_IMGUI_RUNNER_H
#define IMGUI_UTILS_IMGUI_RUNNER_H
#include <string>
#include <imgui.h>
#include <functional>
#include <imgui_examples/details/abstract_runner.h>
#include <memory>

struct GLFWwindow;

namespace ImGuiUtils
{
namespace ImGuiRunner2
{
  // GuiFunction : any function that displays widgets using ImGui
  using GuiFunction = std::function<void(void)>;

  // GuiFunction : any function that displays widgets using ImGui
  // and returns true if exit is desired
  using GuiFunctionWithExit = std::function<bool(void)>;

  // PostInitFunction : any function that will be called once
  // OpenGl, Glfw, Glad and ImGui are inited
  using PostInitFunction = std::function<void(void)>;

  inline void EmptyPostInitFunction() {}

  enum class DefaultWindowTypeOption { ProvideFullScreenWindow, ProvideFullScreenDockSpace, NoDefaultWindow };

  struct AppWindowParams
  {
    bool FullScreen = false;

    bool WindowedFullScreen = false;
    int WindowedFullScreen_HeightReduce = 64;

    int Width = 1280;
    int Height = 720;
    std::string Title = "My Window";
    ImVec4 ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    DefaultWindowTypeOption DefaultWindowType;

    bool ShowMenuBar = false;
    bool ConfigWindowsMoveFromTitleBarOnly = true;
    bool LoadFontAwesome = true;

    std::function<void(ImGuiID /* fullDockSpace */)> InitialDockLayoutFunction;
  };

  using ImGuiRunnerFunctionType = std::function<void(GuiFunctionWithExit, AppWindowParams, PostInitFunction)>;

//  void RunGui_WithExit( // type is ImGuiRunnerFunctionType
//    GuiFunctionWithExit guiFunction,
//    AppWindowParams appWindowParams = AppWindowParams(),
//    PostInitFunction postInitFunction = EmptyPostInitFunction
//    );
//  void RunGui(
//    GuiFunction guiFunction,
//    AppWindowParams appWindowParams = AppWindowParams(),
//    PostInitFunction postInitFunction = EmptyPostInitFunction
//  );
//
//  void ResetDockLayout();


class RunnerBabylon
{
public:
  RunnerBabylon(
    std::unique_ptr<ImGui::ImGuiRunner::AbstractRunner> abstractRunner,
    const AppWindowParams & appWindowParams,
    GuiFunctionWithExit guiFunctionWithExit,
    PostInitFunction postInitFunction = EmptyPostInitFunction
    );

  void Run();
private:
  std::unique_ptr<ImGui::ImGuiRunner::AbstractRunner> mAbstractRunner;
  AppWindowParams mAppWindowParams;
  GuiFunctionWithExit mGuiFunctionWithExit;
  PostInitFunction mPostInitFunction;
};

} // namespace ImGuiRunner
} // namespace ImGuiUtils

#endif // #ifdef IMGUI_UTILS_IMGUI_RUNNER_H
