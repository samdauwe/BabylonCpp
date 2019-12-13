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
  //
  // Public API:
  //

  // Step 1: Construct a Runner (for example RunnerSdl or RunnerGlfw)
  AbstractRunner() = default;
  virtual ~AbstractRunner() = default;

  // Step 2: Change ShowGui by a function pointer (or lambda) that implements your gui and returns true if exit is required
  GuiFunctionPointer ShowGui = ExampleGui;

  // Step 3: (Optionnal): Provide your own own font loading function
  VoidFunctionPointer LoadFonts = LoadNoAdditionalFont;

  // Step 4: Call Run()
  virtual void Run();

  // Public setters
  void SetBackendWindowPosition(ImVec2 position) { mBackendWindowPosition = position; }
  void SetBackendWindowSize(ImVec2 size) { mBackendWindowSize = size; }
  void SetBackendWindowTitle(const std::string& title) { mBackendWindowTitle = title; }
  void SetBackendFullScreen(bool v) { mBackendFullScreen = v; }
  virtual void SetExitRequired(bool v) { mExitRequired = v; };
  void SetClearColor(const ImVec4& color) { mClearColor = color; }

protected:
  //
  // The methods below may be astract and /or overriden in the concrete implementations by derivates
  //
  virtual void InitBackend() = 0;
  virtual void Select_Gl_Version() = 0;
  virtual std::string GlslVersion() = 0;

  virtual void CreateWindowAndContext() = 0;
  virtual void SetupImgGuiContext();
  void SetupImGuiStyle();
  virtual void InitGlLoader() = 0;
  virtual void SetupPlatformRendererBindings() = 0;

  virtual void PollEvents() = 0;
  virtual void NewFrame_OpenGl() = 0; // this could also be vulkan
  virtual void NewFrame_Backend() = 0;
  virtual void Frame_OpenGl_ClearColor() = 0;
  virtual void RenderDrawData_To_OpenGl() = 0;
  virtual void UpdateAndRenderAdditionalPlatformWindows() = 0;
  virtual void SwapBuffers() = 0;
  virtual void Cleanup() = 0;

protected:
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
