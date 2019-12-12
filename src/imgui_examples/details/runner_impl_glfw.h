#ifndef BABYLONCPP_RUNNER_IMPL_GLFW_H
#define BABYLONCPP_RUNNER_IMPL_GLFW_H

#include "imgui_examples/details/arunner.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// SDL + OpenGL3 + glad

namespace ImGuiUtils {
namespace ImGuiRunner {

class RunnerImplGlfw: public ARunner {
public:
  virtual ~RunnerImplGlfw() = default;


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

private:
  GLFWwindow* mWindow = nullptr;
};

} // namespace ImGuiUtils
} // namespace ImGuiRunner

#endif // BABYLONCPP_RUNNER_IMPL_GLFW_H
