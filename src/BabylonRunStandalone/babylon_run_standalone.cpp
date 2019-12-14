#include "babylon_run_standalone.h"
#include <babylon/babylon_imgui/imgui_scene_widget.h>
#include <imgui_runner_babylon/runner_babylon.h>
#include <imgui_runner/runner_sdl.h>
#include <imgui_runner/runner_glfw.h>
#include <imgui_runner/runner_emscripten.h>

// This file demonstrates how to run a standalone scene, using imgui as a backend
void runStandalone(std::shared_ptr<BABYLON::IRenderableScene> scene, bool fullscreen)
{
  // First, set the applications params (title, size, options)
  ImGuiUtils::ImGuiRunner::AppWindowParams appWindowParams;
  appWindowParams.Title = scene->getName();
  appWindowParams.Width = 1280;
  appWindowParams.Height = 720;
  appWindowParams.FullScreen = fullscreen;

  // BABYLON::SceneWidget_ImGui is am ImGui widget that will display the scene
  // It *cannot* be initialized before Glfw and ImGui
  std::unique_ptr<BABYLON::ImGuiSceneWidget> sceneWidget;

  // This lambda will init the scene widget *after* Glfw and ImGui are inited
  auto initScene = [&]() {
    ImVec2 sceneSize = ImGui::GetIO().DisplaySize;
    sceneWidget = std::make_unique<BABYLON::ImGuiSceneWidget>(sceneSize);
    sceneWidget->setRenderableScene(scene);
  };

  // This lambda will display the Gui and it will be called at each frame
  auto showGuiLambda = [&]() {
    // This will display the 3d scene, and handle the mouse & keyboard events
    ImVec2 widgetSize(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
    sceneWidget->render(widgetSize);
    return false;
  };

  // this will run the application
  ImGuiUtils::ImGuiRunner::InvokeRunnerBabylon(
    appWindowParams,
    showGuiLambda,
    initScene
  );

}
