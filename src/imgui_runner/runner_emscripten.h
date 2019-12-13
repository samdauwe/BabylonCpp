#ifndef IMGUI_RUNNER_EMSCRIPTEN_H
#define IMGUI_RUNNER_EMSCRIPTEN_H

#ifdef __EMSCRIPTEN__

#include "imgui_runner/runner_sdl.h"

namespace ImGui {
namespace ImGuiRunner {

class RunnerEmscripten : public RunnerSdl {
public:
  virtual ~RunnerEmscripten() = default;

  void Run() override;

  void Select_Gl_Version() override;
  std::string GlslVersion() override;
  void InitGlLoader() override;
  void SetupImgGuiContext() override;

private:
};

} // namespace ImGuiRunner
} // namespace ImGui


#endif // #ifdef __EMSCRIPTEN__
#endif // IMGUI_RUNNER_EMSCRIPTEN_H
