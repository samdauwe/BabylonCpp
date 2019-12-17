#include "imgui_utils/imgui_runner/runner_sdl.h"
#include "example_gui.h"

int main()
{
  ImGui::ImGuiRunner::RunnerSdl runner;
  runner.ShowGui = ExampleGui;
  runner.Run();
}
