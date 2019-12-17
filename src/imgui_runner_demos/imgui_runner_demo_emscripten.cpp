#include "example_gui.h"
#include "imgui_utils/imgui_runner/runner_emscripten.h"

int main()
{
  ImGui::ImGuiRunner::RunnerEmscripten runner;
  runner.ShowGui = ExampleGui;
  runner.Run();
}
