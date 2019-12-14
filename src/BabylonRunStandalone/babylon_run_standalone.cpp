#include "babylon_run_standalone.h"
#include <babylon/babylon_imgui/imgui_scene_widget.h>
#include <imgui_runner_babylon/runner_babylon.h>
#include <imgui_runner/runner_sdl.h>
#include <imgui_runner/runner_glfw.h>
#include <imgui_runner/runner_emscripten.h>

struct AppContext
{
  std::shared_ptr<BABYLON::IRenderableScene> scene;
  // BABYLON::SceneWidget_ImGui is am ImGui widget that will display the scene
  // It *cannot* be initialized before Glfw and ImGui
  std::shared_ptr<BABYLON::ImGuiSceneWidget> sceneWidget;
};

// When running with emscripten, the main loop is external, so that we need to keep a global variable
AppContext appContext;

// This file demonstrates how to run a standalone scene, using imgui as a backend
void runStandalone(std::shared_ptr<BABYLON::IRenderableScene> scene_, bool fullscreen)
{
  appContext.scene = scene_;

  // First, set the applications params (title, size, options)
  ImGuiUtils::ImGuiRunner::AppWindowParams appWindowParams;
  appWindowParams.Title = appContext.scene->getName();
  appWindowParams.Width = 1280;
  appWindowParams.Height = 720;
  appWindowParams.FullScreen = fullscreen;

  // This lambda will init the scene widget *after* Glfw and ImGui are inited
  auto initScene = []() {
    ImVec2 sceneSize = ImGui::GetIO().DisplaySize;
    appContext.sceneWidget = std::make_shared<BABYLON::ImGuiSceneWidget>(sceneSize);
    appContext.sceneWidget->setRenderableScene(appContext.scene);
  };

  // This lambda will display the Gui and it will be called at each frame
  auto showGuiLambda = []() {
    // This will display the 3d scene, and handle the mouse & keyboard events
    ImVec2 widgetSize(ImGui::GetIO().DisplaySize.x, ImGui::GetIO().DisplaySize.y);
    appContext.sceneWidget->render(widgetSize);
    return false;
  };

  // this will run the application
  ImGuiUtils::ImGuiRunner::InvokeRunnerBabylon(
    appWindowParams,
    showGuiLambda,
    initScene
  );

}
