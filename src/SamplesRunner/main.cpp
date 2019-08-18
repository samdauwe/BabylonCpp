#include <map>
#include <imgui_utils/app_runner/imgui_runner.h>
#include <babylon/imgui_babylon/scene_widget_Imgui.h>
#include <babylon/inspector/inspector.h>
#include <babylon/imgui_babylon/sample_list_page.h>
#include <imgui_utils/icons_font_awesome_5.h>
#include "SamplesRunner/HelloScene.h"
#include "chdir_to_exe.h"

enum class ViewState
{
  Scene3d,
  CodeEditor,
  SampleBrowser,
};

std::map<ViewState, std::string> ViewStatesNames{
  { ViewState::Scene3d, ICON_FA_CUBE " 3D Scene"},
  { ViewState::CodeEditor, ICON_FA_EDIT " Code Editor"},
  { ViewState::SampleBrowser, ICON_FA_PALETTE " Browse samples"},
};

template<typename EnumType> 
bool ShowTabBarEnum(const std::map<EnumType, std::string> & enumNames, EnumType * value)
{
  bool changed = false;
  for (const auto & kv : enumNames)
  {
    bool selected = (*value == kv.first);
    if (selected)
      ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.3f, 0.3f, 0.6f, 1.f));

    if (ImGui::Button(kv.second.c_str()))
    {
      if (*value != kv.first)
      {
        *value = kv.first;
        changed = true;
      }
    }
    if (selected)
      ImGui::PopStyleColor();
    ImGui::SameLine();
  }
  ImGui::NewLine();
  return changed;
}


struct AppContext
{
  std::unique_ptr<BABYLON::SceneWidget_ImGui> _sceneWidget;
  BABYLON::Inspector _inspector;
  BABYLON::SampleListPage _sampleListComponent;
  ViewState _viewState = ViewState::Scene3d;
};

void MyGui(AppContext & appContext)
{
  appContext._inspector.render(false);
  ImGui::SameLine();

  ImGui::BeginGroup();
  ImGui::Text("%s", appContext._sceneWidget->getRenderableScene()->getName());
  ImGui::SameLine(0., 200.);

  ShowTabBarEnum(ViewStatesNames, &appContext._viewState);

  //if (appContext._viewState == ViewState::Scene3d)
  //  appContext._sceneWidget->render();
  if (appContext._viewState == ViewState::CodeEditor)
    ImGui::Text("editor");
  else if (appContext._viewState == ViewState::SampleBrowser)
    appContext._sampleListComponent.render();
  appContext._sceneWidget->render();

  ImGui::EndGroup();
}


int main(int , char** argv)
{
  ChdirToExe(argv[0]);
  AppContext appContext;

  auto OnNewRenderableScene = [&](std::shared_ptr<BABYLON::IRenderableScene> scene) {
    appContext._inspector.setScene(nullptr);
    appContext._sceneWidget->setRenderableScene(scene);
    appContext._inspector.setScene(appContext._sceneWidget->getScene());
  };

  auto initScene = [&]() {
    appContext._sampleListComponent.OnNewRenderableScene = OnNewRenderableScene;

    appContext._sceneWidget = std::make_unique<BABYLON::SceneWidget_ImGui>();
    OnNewRenderableScene(SamplesRunner::MakeHelloScene());
    //appContext._sceneWidget->setRenderableScene(SamplesRunner::MakeHelloScene());
    //appContext._inspector.setScene(appContext._sceneWidget->getScene());
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
