#include "runner_babylon_demo.h"
#include "imgui_utils/imgui_runner/runner_emscripten.h"

int main()
{
  auto runner = std::make_unique<ImGui::ImGuiRunner::RunnerEmscripten>();
  ShowDemo(std::move(runner));
  return 0;
}