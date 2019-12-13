#include "imgui_runner/runner_glfw.h"
#include "example_gui.h"

int main()
{
  ImGui::ImGuiRunner::RunnerGlfw runner;
  runner.ShowGui = ExampleGui;
  runner.Run();
}