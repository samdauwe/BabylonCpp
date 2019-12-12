#ifndef BABYLONCPP_ABSTRACT_RUNNER_H
#define BABYLONCPP_ABSTRACT_RUNNER_H

#include <string>
#include <imgui.h>

namespace ImGui {
namespace ImGuiRunner {

ImVec2 MainScreenResolution();

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
  void LoadFonts();

  void SetClearColor(const ImVec4& color) { mClearColor = color; }
  virtual bool ExitRequired() = 0;
  virtual void PollEvents() = 0;
  virtual void NewFrame_OpenGl() = 0; // this could also be vulkan
  virtual void NewFrame_Backend() = 0;
  void ImGuiRender();
  virtual void RenderDrawData_To_OpenGl() = 0;
  virtual void UpdateAndRenderAdditionalPlatformWindows() = 0;
  virtual void SwapBuffers() = 0;
  virtual void Cleanup() = 0;


  // override this function with your own gui!
  virtual void ShowGui() = 0;

  void RunIt();

protected:
  std::string mBackendWindowTitle = "Dear ImGui example";
  ImVec2 mBackendWindowPosition = ImVec2(-1.f, -1.f);
  ImVec2 mBackendWindowSize = MainScreenResolution();
  bool mBackendFullScreen = false;
  ImVec4 mClearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};

} // namespace ImGuiRunner
} // namespace ImGui

#endif // BABYLONCPP_ABSTRACT_RUNNER_H
