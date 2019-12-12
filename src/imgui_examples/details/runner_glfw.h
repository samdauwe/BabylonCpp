#ifndef BABYLONCPP_RUNNER_GLFW_H
#define BABYLONCPP_RUNNER_GLFW_H
#ifdef BABYLON_USE_GLFW
#include "imgui_examples/details/arunner.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// SDL + OpenGL3 + glad

namespace ImGuiUtils {
namespace ImGuiRunner {

class ImGuiRunnerGlfw : public ARunner {
public:
  virtual ~ImGuiRunnerGlfw() = default;

  void InitBackend() override;
  void Select_Gl_Version() override;
  std::string GlslVersion() override;
  void CreateWindowAndContext() override;
  void InitGlLoader() override;
  void SetupPlatformRendererBindings() override;

  bool ExitRequired() override;
  void PollEvents() override;
  void NewFrame_OpenGl() override;
  void NewFrame_Backend() override;
  void RenderDrawData_To_OpenGl() override;
  void UpdateAndRenderAdditionalPlatformWindows() override;
  void Cleanup() override;
  void SwapBuffers() override;

private:
  GLFWwindow* mWindow = nullptr;
};

} // namespace ImGuiUtils
} // namespace ImGuiRunner
#endif // #ifdef BABYLON_USE_GLFW
#endif // BABYLONCPP_RUNNER_GLFW_H
