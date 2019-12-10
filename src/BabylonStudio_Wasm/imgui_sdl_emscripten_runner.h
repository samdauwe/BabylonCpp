#ifndef BABYLONCPP_IMGUI_SDL_EMSCRIPTEN_RUNNER_H
#define BABYLONCPP_IMGUI_SDL_EMSCRIPTEN_RUNNER_H

#include <imgui_utils/app_runner/imgui_runner.h>

namespace ImGuiUtils
{
namespace ImGuiRunner
{
  void RunGui_Emscripten(
    GuiFunctionWithExit guiFunction,
    AppWindowParams appWindowParams = AppWindowParams(),
    PostInitFunction postInitFunction = EmptyPostInitFunction
  );

} // namespace ImGuiRunner
} // namespace ImGuiUtils

#endif // BABYLONCPP_IMGUI_SDL_EMSCRIPTEN_RUNNER_H
