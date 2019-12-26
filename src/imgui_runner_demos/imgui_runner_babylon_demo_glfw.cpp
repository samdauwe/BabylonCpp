#include "runner_babylon_demo.h"
#include "imgui_utils/imgui_runner/runner_glfw.h"

int main()
{
  auto runner = std::make_unique<ImGui::ImGuiRunner::RunnerGlfw>();
  ShowDemo(std::move(runner));
  return 0;
}
