#ifndef BABYLONCPP_ARUNNER_H
#define BABYLONCPP_ARUNNER_H

#include <string>
#include <imgui.h>

namespace ImGuiUtils {
namespace ImGuiRunner {

class ARunner {
public:
  ARunner() = default;
  virtual ~ARunner() = default;

  virtual void InitBackend() = 0;
  virtual void Select_Gl_Version() = 0;
  virtual std::string GlslVersion() = 0;
  virtual void CreateWindowAndContext() = 0;
  void SetupImgGuiContext();
  void SetupImGuiStyle();
  virtual void InitGlLoader() = 0;
  virtual void SetupPlatformRendererBindings() = 0;
  void LoadFonts();

  ImVec4 ClearColor();
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
};

} // namespace ImGuiUtils
} // namespace ImGuiRunner

#endif // BABYLONCPP_ARUNNER_H
