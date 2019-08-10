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

    struct WindowParams
    {
      bool FullScreen = false;
      int Width = 1280;
      int Height = 720;
      std::string Title = "My Window";
      GLFWwindow *ParentWindow = nullptr;
      ImVec4 ClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
    };

    void RunGui(
      GuiFunctionWithExit guiFunction,
      WindowParams windowParams = WindowParams(),
      PostInitFunction postInitFunction = EmptyPostInitFunction
      );
    void RunGui(
      GuiFunction guiFunction,
      WindowParams windowParams = WindowParams(),
      PostInitFunction postInitFunction = EmptyPostInitFunction
    );


  }
}
