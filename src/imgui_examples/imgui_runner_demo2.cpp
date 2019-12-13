#include "imgui.h"
#include "imgui_internal.h"
#include <imgui_examples/details/runner_glfw.h>
#include <imgui_examples/details/runner_sdl.h>
#include <imgui_examples/runner_babylon.h>

#include <map>
#include <string>
#include <vector>

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

bool DummyWindow(const char* title)
{
  bool shouldExit = false;
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
      ImGuiUtils::ImGuiRunner2::RunnerBabylon::ResetDockLayout();

    if (ImGui::Button("Quit"))
      shouldExit = true;

    ImGui::End();
  }
  return shouldExit;
}

bool DemoGui()
{
  bool shouldExit = false;
  std::vector<std::string> titles
    = {"Left", "LeftBottom1", "LeftBottom2", "LeftBottom3", "Right", "Main", "Bottom"};
  for (const auto& title : titles)
    if (DummyWindow((title + "").c_str()))
      shouldExit = true;
  return shouldExit;
}

void ShowDemo()
{
  using namespace ImGuiUtils::ImGuiRunner2;
  AppWindowParams params;
  params.DefaultWindowType         = DefaultWindowTypeOption::ProvideFullScreenDockSpace;
  params.InitialDockLayoutFunction = MyCreateDockLayout;
  params.Title                     = "Hello World";
  params.Width = 1280;
  params.Height = 720;
  params.FullScreen = true;
  //params.WindowedFullScreen = true;
  //ImGuiUtils::ImGuiRunner::RunGui(DemoGui, params);

#ifdef IMGUI_RUNNER_USE_GLFW
  auto runnerGlfw = std::make_unique<ImGui::ImGuiRunner::RunnerGlfw>();
#else
  auto runnerGlfw = std::make_unique<ImGui::ImGuiRunner::RunnerSdl>();
#endif
  ImGuiUtils::ImGuiRunner2::RunnerBabylon runnerBabylon(
    std::move(runnerGlfw),
    params,
    DemoGui
  );
  runnerBabylon.Run();
}


int main()
{
  ShowDemo();
  return 0;
}


