#include <imgui_utils/icons_font_awesome_5.h>
#include <babylon/inspector/babylon_inspector_app.h>
#include <imgui_utils/app_runner/imgui_runner.h>

namespace BABYLON
{
  const int INSPECTOR_WIDTH = 400;

  namespace
  {
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

  }

  std::map<BabylonInspectorApp::ViewState, std::string> BabylonInspectorApp::ViewStateLabels = {
  { BabylonInspectorApp::ViewState::Scene3d, ICON_FA_CUBE " 3D Scene"},
  { BabylonInspectorApp::ViewState::CodeEditor, ICON_FA_EDIT " Code Editor"},
  { BabylonInspectorApp::ViewState::SampleBrowser, ICON_FA_PALETTE " Browse samples"},
  };


  void BabylonInspectorApp::render()
  {
    _appContext._inspector.render(false, INSPECTOR_WIDTH);
    ImGui::SameLine();

    ImGui::BeginGroup();
    ImGui::Text("%s", _appContext._sceneWidget->getRenderableScene()->getName());
    ImGui::SameLine(0., 200.);

    ShowTabBarEnum(ViewStateLabels, &_appContext._viewState);

    if (_appContext._viewState == ViewState::Scene3d)
      _appContext._sceneWidget->render();
    if (_appContext._viewState == ViewState::CodeEditor)
      ImGui::Text("editor");
    else if (_appContext._viewState == ViewState::SampleBrowser)
      _appContext._sampleListComponent.render();
    //_appContext._sceneWidget->render();

    ImGui::EndGroup();
  }

  void BabylonInspectorApp::setRenderableScene(std::shared_ptr<BABYLON::IRenderableScene> scene)
  {
    _appContext._inspector.setScene(nullptr);
    _appContext._sceneWidget->setRenderableScene(scene);
    _appContext._inspector.setScene(_appContext._sceneWidget->getScene());
    _appContext._viewState = ViewState::Scene3d;
  }

  BabylonInspectorApp::BabylonInspectorApp()
  {
  }

  void BabylonInspectorApp::RunApp(std::shared_ptr<BABYLON::IRenderableScene> initialScene /*= nullptr*/)
  {
    ImGuiUtils::ImGuiRunner::AppWindowParams params;
    //params.FullScreen = true;
    params.Width = 1280;
    params.Height = 720;
    params.Title = "Hello app";
    auto showGuiLambda = [this]() {
      this->render();
    };
    auto initSceneLambda = [&]() {
      this->initScene();
      this->setRenderableScene(initialScene);
    };
    ImGuiUtils::ImGuiRunner::RunGui(showGuiLambda, params, initSceneLambda);
  }

  void BabylonInspectorApp::initScene()
  {
    _appContext._sampleListComponent.OnNewRenderableScene = [&](std::shared_ptr<IRenderableScene> scene) {
      this->setRenderableScene(scene);
    };

    ImVec2 sceneSize = ImGui::GetIO().DisplaySize;
    sceneSize.x -= INSPECTOR_WIDTH;
    sceneSize.y -= 60;

    _appContext._sceneWidget = std::make_unique<BABYLON::SceneWidget_ImGui>(sceneSize);
  }

}