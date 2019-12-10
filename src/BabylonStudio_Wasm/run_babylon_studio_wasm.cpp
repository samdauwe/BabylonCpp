#include "imgui_sdl_emscripten_runner.h"
#include <babylon/babylon_imgui/babylon_studio.h>

int main(int, char**)
{
  BABYLON::runBabylonStudio(
    nullptr,
    BABYLON::BabylonStudioOptions(),
    ImGuiUtils::ImGuiRunner::RunGui_Emscripten
  );
}

