#include "imgui.h"
#include "imgui_internal.h"
#include <imgui_utils/app_runner/imgui_runner.h>

#include <map>
#include <string>
#include <vector>

namespace ImGuiUtils {
namespace ImGuiRunner {
void MyCreateDockLayout(ImGuiID fullDockSpaceId)
{
  ImGuiViewport* viewport = ImGui::GetMainViewport();
  ImGui::DockBuilderRemoveNode(fullDockSpaceId); // Clear out existing layout
  ImGui::DockBuilderAddNode(fullDockSpaceId);    // Add empty node
  ImGui::DockBuilderSetNodeSize(fullDockSpaceId, viewport->Size);

  ImGuiID dock_main_id
    = fullDockSpaceId; // This variable will track the document node, however we are not using it
                       // here as we aren't docking anything into it.
  ImGuiID dock_id_left
    = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.20f, nullptr, &dock_main_id);
  ImGuiID dock_id_right
    = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.20f, nullptr, &dock_main_id);
  ImGuiID dock_id_bottom
    = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.20f, nullptr, &dock_main_id);

  ImGuiID dock_id_left_bottom
    = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.50f, nullptr, &dock_id_left);

  ImGui::DockBuilderDockWindow("Left", dock_id_left);
  ImGui::DockBuilderDockWindow("LeftBottom1", dock_id_left_bottom);
  ImGui::DockBuilderDockWindow("LeftBottom2", dock_id_left_bottom);
  ImGui::DockBuilderDockWindow("LeftBottom3", dock_id_left_bottom);
  ImGui::DockBuilderDockWindow("Main", dock_main_id);
  ImGui::DockBuilderDockWindow("Right", dock_id_right);
  ImGui::DockBuilderDockWindow("Bottom", dock_id_bottom);
  ImGui::DockBuilderFinish(fullDockSpaceId);
}

void DummyWindow(const char* title)
{
  static std::map<std::string, bool> openStatuses;
  if (openStatuses.find(title) == openStatuses.end())
    openStatuses[title] = true;

  ImGui::SetNextWindowSize(ImVec2(300, 300), ImGuiCond_Once);
  bool& dummyOpen = openStatuses.at(title);
  int windowFlags = 0; // ImGuiWindowFlags_NoMove;
  if (dummyOpen) {
    ImGui::Begin(title, &dummyOpen, windowFlags);
    ImGui::Text("%s", title);

    static bool show_demo_window = false;
    ImGui::Checkbox("Demo Window", &show_demo_window);
    if (show_demo_window)
      ImGui::ShowDemoWindow(&show_demo_window);

    if (ImGui::Button("Reset Layout"))
      ImGuiRunner::ResetDockLayout();

    ImGui::End();
  }
}
void DemoGui()
{
  std::vector<std::string> titles
    = {"Left", "LeftBottom1", "LeftBottom2", "LeftBottom3", "Right", "Main", "Bottom"};
  for (const auto& title : titles)
    DummyWindow((title + "").c_str());
}

void ShowDemo()
{
  ImGuiUtils::ImGuiRunner::AppWindowParams params;
  params.DefaultWindowType         = DefaultWindowTypeOption::ProvideFullScreenDockSpace;
  params.InitialDockLayoutFunction = MyCreateDockLayout;
  params.Title                     = "Hello World";
  ImGuiUtils::ImGuiRunner::RunGui(DemoGui, params);
}
} // namespace ImGuiRunner
} // namespace ImGuiUtils


int main1()
{
  //ImGuiUtils::ImGuiRunner::ShowDemo();
  return 0;
}


#include <imgui_examples/details/runner_glfw.h>

#ifdef BABYLON_USE_GLFW
class MyRunner: public ImGuiUtils::ImGuiRunner::ImGuiRunnerGlfw
{
public:
void ShowGui()
{
  static bool show_demo_window = true;
  if (show_demo_window)
    ImGui::ShowDemoWindow(&show_demo_window);
}

};
#endif

int main()
{
#ifdef BABYLON_USE_GLFW
  MyRunner runner;
  runner.RunIt();
#endif
  return 0;
}