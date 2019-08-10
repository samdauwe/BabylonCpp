#include <imgui_utils/app_runner/imgui_runner.h>
#include "SamplesRunner/HelloScene.h"
#include "SamplesRunner/scene_widget_imgui.h"

struct AppContext
{
  std::unique_ptr<BABYLON::SceneWidget_ImGui> _sceneWidget;
};

void MyGui(AppContext & appContext)
{
  //ImGui::Text("Hello");
  appContext._sceneWidget->render();
  //ImGui::Text("end");
}

int main(int, char**)
{
  AppContext appContext;  
  auto initScene = [&]() {
    appContext._sceneWidget = std::make_unique<BABYLON::SceneWidget_ImGui>();
    appContext._sceneWidget->setScene(SamplesRunner::MakeHelloScene());
  };

  auto showGui = [&]() {    
    MyGui(appContext);
  };

  ImGuiUtils::ImGuiRunner::AppWindowParams params;
  //params.FullScreen = true;
  params.Width = 800;
  params.Height = 600;
  params.Title = "Hello app";
  ImGuiUtils::ImGuiRunner::RunGui(showGui, params, initScene);
  return 0;
}
