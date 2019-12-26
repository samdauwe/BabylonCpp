#ifndef IMGUI_RUNNER_SDL_H
#define IMGUI_RUNNER_SDL_H
#ifdef IMGUI_RUNNER_USE_SDL
#include "imgui_utils/imgui_runner/abstract_runner.h"

#include <glad/glad.h>
#include <SDL.h>

// SDL + OpenGL3 + glad

namespace ImGui {
namespace ImGuiRunner {

class RunnerSdl : public AbstractRunner {
public:
  virtual ~RunnerSdl() = default;

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
  SDL_Window* mWindow = nullptr;
  SDL_GLContext mGlContext = nullptr;
};

} // namespace ImGuiRunner
} // namespace ImGui

#endif // #ifdef IMGUI_RUNNER_USE_SDL
#endif // IMGUI_RUNNER_SDL_H
