#ifndef IMGUI_UTILS_IMGUI_RUNNER_H
#define IMGUI_UTILS_IMGUI_RUNNER_H

#include <string>
#include <imgui.h>
#include <functional>

struct GLFWwindow;

namespace ImGuiUtils
{
namespace ImGuiRunner
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
    GLFWwindow *ParentWindow = nullptr;
    ImVec4 ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    DefaultWindowTypeOption DefaultWindowType;

    bool ShowMenuBar = false;
    bool ConfigWindowsMoveFromTitleBarOnly = true;
    bool LoadFontAwesome = true;

    std::function<void(ImGuiID /* fullDockSpace */)> InitialDockLayoutFunction;
  };

  void RunGui(
    GuiFunctionWithExit guiFunction,
    AppWindowParams appWindowParams = AppWindowParams(),
    PostInitFunction postInitFunction = EmptyPostInitFunction
    );
  void RunGui(
    GuiFunction guiFunction,
    AppWindowParams appWindowParams = AppWindowParams(),
    PostInitFunction postInitFunction = EmptyPostInitFunction
  );

  void ResetDockLayout();

} // namespace ImGuiRunner
} // namespace ImGuiUtils

#endif // #ifdef IMGUI_UTILS_IMGUI_RUNNER_H
