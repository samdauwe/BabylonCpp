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

    struct AppWindowParams
    {
      bool FullScreen = false;
      int Width = 1280;
      int Height = 720;
      std::string Title = "My Window";
      GLFWwindow *ParentWindow = nullptr;
      ImVec4 ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
      bool ProvideFullScreenWindow = true;
      bool ShowMenuBar = false;
      bool ConfigWindowsMoveFromTitleBarOnly = true;
      bool LoadFontAwesome = true;
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


  }
}
