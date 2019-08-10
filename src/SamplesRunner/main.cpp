#include <imgui_utils/app_runner/imgui_runner.h>
#include "SamplesRunner/HelloScene.h"
#include "SamplesRunner/imgui_scene_renderer.h"

struct AppContext
{
  std::unique_ptr<BABYLON::ImGuiRender::ImGuiSceneRenderer> _imguiRenderer;
  std::shared_ptr<BABYLON::IRenderableScene> _scene;
};

void MyGui(AppContext & appContext)
{
  ImGui::Begin("Win", NULL, ImGuiWindowFlags_NoMove);
  ImGui::Text("Hello");
  appContext._imguiRenderer->showGui();
  ImGui::Text("end");
  ImGui::End();
}

int main(int, char**)
{
  AppContext appContext;  
  auto initScene = [&]() {
    appContext._imguiRenderer = std::make_unique<BABYLON::ImGuiRender::ImGuiSceneRenderer>(640, 480);
    appContext._scene = std::make_shared<SamplesRunner::HelloScene>(appContext._imguiRenderer->getCanvas());
    appContext._imguiRenderer->setScene(appContext._scene.get());
  };

  auto showGui = [&]() {
    MyGui(appContext);
  };

  ImGuiUtils::ImGuiRunner::WindowParams params;
  //params.FullScreen = true;
  params.Width = 800;
  params.Height = 600;
  params.Title = "Hello app";
  ImGuiUtils::ImGuiRunner::RunGui(showGui, params, initScene);
  return 0;
}
