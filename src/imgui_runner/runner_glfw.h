#ifndef IMGUI_RUNNER_GLFW_H
#define IMGUI_RUNNER_GLFW_H
#ifdef IMGUI_RUNNER_USE_GLFW
#include "imgui_runner/abstract_runner.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// GLFW + OpenGL3 + glad

namespace ImGui {
namespace ImGuiRunner {

class RunnerGlfw : public AbstractRunner {
public:
  virtual ~RunnerGlfw() = default;

protected:
  void InitBackend() override;
  void Select_Gl_Version() override;
  std::string GlslVersion() override;
  void CreateWindowAndContext() override;
  void InitGlLoader() override;
  void SetupPlatformRendererBindings() override;

  void PollEvents() override;
  void NewFrame_OpenGl() override;
  void NewFrame_Backend() override;
  void Frame_OpenGl_ClearColor() override;
  void RenderDrawData_To_OpenGl() override;
  void UpdateAndRenderAdditionalPlatformWindows() override;
  void Cleanup() override;
  void SwapBuffers() override;

private:
  GLFWwindow* mWindow = nullptr;
};

} // namespace ImGuiRunner
} // namespace ImGui
#endif // #ifdef IMGUI_RUNNER_USE_GLFW
#endif // IMGUI_RUNNER_GLFW_H
