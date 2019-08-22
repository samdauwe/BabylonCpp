#include <imgui_utils/icons_font_awesome_5.h>
#include <babylon/inspector/babylon_inspector_app.h>
#include <imgui_utils/app_runner/imgui_runner.h>
#include <babylon/GL/framebuffer_canvas.h>

#include <babylon/core/logging.h>
#include <babylon/samples/samples_index.h>

#ifdef _WIN32
#include <windows.h>
#endif
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
      ImGui::Separator();
      return changed;
    }

  }

  std::map<BabylonInspectorApp::ViewState, std::string> BabylonInspectorApp::ViewStateLabels = {
  { BabylonInspectorApp::ViewState::Scene3d, ICON_FA_CUBE " 3D Scene"},
  { BabylonInspectorApp::ViewState::CodeEditor, ICON_FA_EDIT " Code Editor"},
  { BabylonInspectorApp::ViewState::SampleBrowser, ICON_FA_PALETTE " Browse samples"},
  };


  bool BabylonInspectorApp::render()
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
      _codeEditor.render();
    else if (_appContext._viewState == ViewState::SampleBrowser)
      _appContext._sampleListComponent.render();

    //ImGui::ShowDemoWindow(nullptr);

    ImGui::EndGroup();

    // loopSamples();

    if (_appContext._screenshotAndExit)
      return saveScreenshot();
    else
      return false;
  }

  void BabylonInspectorApp::setRenderableScene(std::shared_ptr<BABYLON::IRenderableScene> scene)
  {
    _appContext._inspector.setScene(nullptr);
    _appContext._sceneWidget->setRenderableScene(scene);
    _appContext._inspector.setScene(_appContext._sceneWidget->getScene());
    _appContext._viewState = ViewState::Scene3d;
  }

  bool BabylonInspectorApp::saveScreenshot()
  {
    _appContext._frameCounter++;
    if (_appContext._frameCounter < 30)
      return false;
    int imageWidth = 200;
    int jpgQuality = 75;
    this->_appContext._sceneWidget->getCanvas()->saveScreenshotJpg((_appContext._sceneName + ".jpg").c_str(),
      jpgQuality, imageWidth);
    return true;
  }

  void BabylonInspectorApp::loopSamples()
  {
    static BABYLON::Samples::SamplesIndex samplesIndex;
    static std::vector<std::string> allSamples = samplesIndex.getSampleNames();
    static int frame_counter = 0;
    const int max_frames = 30;

    static size_t sample_counter = 0;

    if (frame_counter > max_frames)
    {
      std::string sampleName = allSamples[sample_counter];
      std::vector<std::string> excludedSamples = {
        //"BasicElementsScene",
        //"BlurModeForMirrorsScene"
      };
      if (std::find(excludedSamples.begin(), excludedSamples.end(), sampleName) == excludedSamples.end())
      {
        BABYLON_LOG_ERROR("LoopSample", sampleName);
        auto scene_unique = samplesIndex.createRenderableScene(sampleName, nullptr);
        std::shared_ptr<BABYLON::IRenderableScene> scene_shared(std::move(scene_unique));
        try
        {
          this->setRenderableScene(scene_shared);
        }
        catch (...)
        {
          this->setRenderableScene(nullptr);
        }
      }

      if (sample_counter < allSamples.size() - 2)
        sample_counter++;

      frame_counter = 0;
    }
    else
      frame_counter++;
  }

  BabylonInspectorApp::BabylonInspectorApp()
  {
  }

  void BabylonInspectorApp::RunApp(
    std::shared_ptr<BABYLON::IRenderableScene> initialScene /*= nullptr*/,
    std::string sceneName /*= ""*/,
    bool screenshotAndExit /*= false*/)
  {
    _appContext._sceneName = sceneName;
    _appContext._screenshotAndExit = screenshotAndExit;

    ImGuiUtils::ImGuiRunner::AppWindowParams params;
    //params.FullScreen = true;
    params.Width = 1280;
    params.Height = 720;
    params.Title = "Hello app";
    std::function<bool(void)> showGuiLambda = [this]() -> bool {
      return this->render();
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
    _appContext._sampleListComponent.OnEditFiles = [&](const std::vector<std::string> & files) {
      _codeEditor.setFiles(files);
      _appContext._viewState = ViewState::CodeEditor;
    };

    ImVec2 sceneSize = ImGui::GetIO().DisplaySize;
    sceneSize.x -= INSPECTOR_WIDTH;
    sceneSize.y -= 60;

    _appContext._sceneWidget = std::make_unique<BABYLON::SceneWidget_ImGui>(sceneSize);
  }

}