#include "run_standalone_imgui.h"
#include <imgui_utils/app_runner/imgui_runner.h>
#include <babylon/babylon_imgui/scene_widget_Imgui.h>


// This file demonstrates how to run a standalone scene, using imgui as a backend
void runStandalone_imgui(
  std::shared_ptr<BABYLON::IRenderableScene> scene, 
  const std::string & sceneName,
  bool fullscreen)
{
  // First, set the applications params (title, size, options)
  ImGuiUtils::ImGuiRunner::AppWindowParams params;
  params.Title = sceneName;
  params.Width = 1280;
  params.Height = 720;
  params.FullScreen = fullscreen;

  // BABYLON::SceneWidget_ImGui is am ImGui widget that will display the scene
  // It *cannot* be initialized before Glfw and ImGui
  std::unique_ptr<BABYLON::SceneWidget_ImGui> sceneWidget;

  // This lambda will init the scene widget *after* Glfw and ImGui are inited
  auto initScene = [&]() {
    ImVec2 sceneSize = ImGui::GetIO().DisplaySize;
    sceneWidget = std::make_unique<BABYLON::SceneWidget_ImGui>(sceneSize);
    sceneWidget->setRenderableScene(scene);
  };

  // This lambda will display the Gui and it will be called at each frame
  auto showGuiLambda = [&]() {
    // you can add any ImGui widget as you wish! Feel free to comment out and or edit this block!
    {
      ImGui::Text("Hello, dear ImGui!"); ImGui::SameLine(0., 50.f);
      static bool showDemoWindow = false;
      ImGui::Checkbox("ShowDemoWindow", &showDemoWindow);
      if (showDemoWindow)
        ImGui::ShowDemoWindow(nullptr);
    }

    // This will display the 3d scene, and handle the mouse & keyboard events
    sceneWidget->render();
  };

  // this will run the application
  ImGuiUtils::ImGuiRunner::RunGui(showGuiLambda, params, initScene);
}
