#ifndef BABYLONCPP_RUNNER_SDL_H
#define BABYLONCPP_RUNNER_SDL_H
#ifdef BABYLON_USE_SDL
#include "imgui_examples/details/arunner.h"

#include <glad/glad.h>
#include <SDL.h>

// SDL + OpenGL3 + glad

namespace ImGuiUtils {
namespace ImGuiRunner {

class ImGuiRunnerSdl : public ARunner {
public:
  virtual ~ImGuiRunnerSdl() = default;

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
  SDL_Window* mWindow = nullptr;
  SDL_GLContext mGlContext = nullptr;
  bool mExitRequired = false;
};

} // namespace ImGuiUtils
} // namespace ImGuiRunner

#endif // #ifdef BABYLON_USE_SDL
#endif // BABYLONCPP_RUNNER_SDL_H
