#include <imgui_utils/app_runner/imgui_runner.h>
#include <babylon/imgui_babylon/scene_widget_Imgui.h>
#include <babylon/inspector/inspector.h>
#include "SamplesRunner/HelloScene.h"
#include "chdir_to_exe.h"

struct AppContext
{
  std::unique_ptr<BABYLON::SceneWidget_ImGui> _sceneWidget;
  BABYLON::Inspector _inspector;
};

void MyGui(AppContext & appContext)
{
  appContext._inspector.render(false);
  ImGui::SameLine();
  appContext._sceneWidget->render();
}


int main(int , char** argv)
{
  ChdirToExe(argv[0]);
  AppContext appContext;  
  auto initScene = [&]() {
    appContext._sceneWidget = std::make_unique<BABYLON::SceneWidget_ImGui>();
    appContext._sceneWidget->setRenderableScene(SamplesRunner::MakeHelloScene());
    appContext._inspector.setScene(appContext._sceneWidget->getScene());
  };

  auto showGui = [&]() {    
    MyGui(appContext);
  };

  ImGuiUtils::ImGuiRunner::AppWindowParams params;
  //params.FullScreen = true;
  params.Width = 1280;
  params.Height = 720;
  params.Title = "Hello app";
  ImGuiUtils::ImGuiRunner::RunGui(showGui, params, initScene);
  return 0;
}
