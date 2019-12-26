#include "runner_babylon_demo.h"
#include "imgui_utils/imgui_runner/runner_sdl.h"

int main()
{
  auto runner = std::make_unique<ImGui::ImGuiRunner::RunnerSdl>();
  ShowDemo(std::move(runner));
  return 0;
}