#include <imgui_utils/icons_font_awesome_5.h>
#include <babylon/babylon_imgui/run_scene_with_inspector.h>
#include <imgui_utils/app_runner/imgui_runner.h>
#include <babylon/GL/framebuffer_canvas.h>

#include <babylon/core/logging.h>
#include <babylon/samples/samples_index.h>

#ifdef _WIN32
#include <windows.h>
#endif
namespace BABYLON
{
namespace impl
{
const int INSPECTOR_WIDTH = 400;

class BabylonInspectorApp {
public:

  BabylonInspectorApp();
  void RunApp(
    std::shared_ptr<BABYLON::IRenderableScene> initialScene,
    const SceneWithInspectorOptions & options
  );

private:
  enum class ViewState
  {
    Scene3d,
    CodeEditor,
    SampleBrowser,
  };
  static std::map<BabylonInspectorApp::ViewState, std::string> ViewStateLabels;

  void initScene();
  bool render(); // renders the GUI. Returns true when exit required
  void setRenderableScene(std::shared_ptr<BABYLON::IRenderableScene> scene);
  // Saves a screenshot after  few frames (eeturns true when done)
  bool saveScreenshot();

private:
  void loopSamples();

  struct AppContext
  {
    std::unique_ptr<BABYLON::ImGuiSceneWidget> _sceneWidget;
    BABYLON::Inspector _inspector;
    BABYLON::SamplesBrowser _sampleListComponent;
    ViewState _viewState = ViewState::Scene3d;
    int _frameCounter = 0;
    SceneWithInspectorOptions _options;
  };

  AppContext _appContext;
  ImGuiUtils::CodeEditor _codeEditor;
}; // end of class BabylonInspectorApp


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

  if (_appContext._options._flagScreenshotOneSampleAndExit)
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
  this->_appContext._sceneWidget->getCanvas()->saveScreenshotJpg((_appContext._options._sceneName + ".jpg").c_str(),
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
  std::shared_ptr<BABYLON::IRenderableScene> initialScene,
  const SceneWithInspectorOptions & options)
{
  _appContext._options = options;

  std::function<bool(void)> showGuiLambda = [this]() -> bool {
    return this->render();
  };
  auto initSceneLambda = [&]() {
    this->initScene();
    this->setRenderableScene(initialScene);
  };
  ImGuiUtils::ImGuiRunner::RunGui(showGuiLambda, _appContext._options._appWindowParams, initSceneLambda);
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

  _appContext._sceneWidget = std::make_unique<BABYLON::ImGuiSceneWidget>(sceneSize);
}

} // namespace impl

// public API
void runSceneWithInspector(
  std::shared_ptr<BABYLON::IRenderableScene> scene,
  SceneWithInspectorOptions options /* = SceneWithInspectorOptions() */
)
{
  BABYLON::impl::BabylonInspectorApp app;
  app.RunApp(scene, options);
}

} // namespace BABYLON
