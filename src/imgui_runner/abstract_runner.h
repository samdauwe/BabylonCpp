#ifndef BABYLONCPP_ABSTRACT_RUNNER_H
#define BABYLONCPP_ABSTRACT_RUNNER_H

#include <string>
#include <imgui.h>
#include <functional>

namespace ImGui {
namespace ImGuiRunner {

ImVec2 MainScreenResolution();

// GuiFunctionPointer ; any function that displays a GUI via ImGui
// and returns false most of the time, except if Quit is required
using GuiFunctionPointer = std::function<bool(void)>;
using VoidFunctionPointer = std::function<void(void)>;

bool ExampleGui();
void LoadNoAdditionalFont();

class AbstractRunner {
public:
  AbstractRunner() = default;
  virtual ~AbstractRunner() = default;

  virtual void InitBackend() = 0;
  virtual void Select_Gl_Version() = 0;
  virtual std::string GlslVersion() = 0;

  void SetBackendWindowPosition(ImVec2 position) { mBackendWindowPosition = position; }
  void SetBackendWindowSize(ImVec2 size) { mBackendWindowSize = size; }
  void SetBackendWindowTitle(const std::string& title) { mBackendWindowTitle = title; }
  void SetBackendFullScreen(bool v) { mBackendFullScreen = v; }

  virtual void CreateWindowAndContext() = 0;

  void SetupImgGuiContext();
  void SetupImGuiStyle();
  virtual void InitGlLoader() = 0;
  virtual void SetupPlatformRendererBindings() = 0;

  void SetClearColor(const ImVec4& color) { mClearColor = color; }
  virtual void SetExitRequired(bool v) { mExitRequired = v; };
  virtual void PollEvents() = 0;
  virtual void NewFrame_OpenGl() = 0; // this could also be vulkan
  virtual void NewFrame_Backend() = 0;
  void ImGuiRender();
  virtual void RenderDrawData_To_OpenGl() = 0;
  virtual void UpdateAndRenderAdditionalPlatformWindows() = 0;
  virtual void SwapBuffers() = 0;
  virtual void Cleanup() = 0;

  // Gui function pointer : replace it by your own Gui function!
  GuiFunctionPointer ShowGui = ExampleGui;
  // Replace this by your own font loading function
  VoidFunctionPointer LoadFonts = LoadNoAdditionalFont;

  void Run();

private:
  friend void emscripten_imgui_main_loop(void *);
  void LoopProc();

  void FullInit();

protected:
  std::string mBackendWindowTitle = "Dear ImGui example";
  ImVec2 mBackendWindowPosition = ImVec2(-1.f, -1.f);
  ImVec2 mBackendWindowSize = MainScreenResolution();
  bool mBackendFullScreen = false;
  ImVec4 mClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  bool mExitRequired = false;
};

} // namespace ImGuiRunner
} // namespace ImGui

#endif // BABYLONCPP_ABSTRACT_RUNNER_H
