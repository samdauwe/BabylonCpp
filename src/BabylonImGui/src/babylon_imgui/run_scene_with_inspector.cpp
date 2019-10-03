#include <imgui_utils/icons_font_awesome_5.h>
#include <babylon/babylon_imgui/run_scene_with_inspector.h>
#include <babylon/babylon_imgui/babylon_logs_window.h>
#include <babylon/interfaces/irenderable_scene_with_hud.h>
#include <imgui_utils/app_runner/imgui_runner.h>
#include <babylon/GL/framebuffer_canvas.h>
#include <babylon/core/filesystem.h>
#include <babylon/core/system.h>
#include <imgui_utils/app_runner/imgui_runner.h>
#include <imgui_utils/icons_font_awesome_5.h>

#include <babylon/core/logging.h>
#include <babylon/samples/samples_index.h>

#ifdef _WIN32
#include <windows.h>
#endif

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
    //ImGui::NewLine();
    //ImGui::Separator();
    return changed;
  }

} // namespace

#include <iostream>

namespace BABYLON
{
namespace impl
{
const int INSPECTOR_WIDTH = 400;

class BabylonInspectorApp {
public:

  BabylonInspectorApp() {
    std::string exePath = BABYLON::System::getExecutablePath();
    std::string exeFolder = BABYLON::Filesystem::baseDir(exePath);
    std::string sandboxPath = exeFolder + "/../../../src/SamplesRunner/sandbox.cpp";
    sandboxPath = BABYLON::Filesystem::absolutePath(sandboxPath);
    _sandboxCodeEditor.setFiles({ sandboxPath });
  }
  void RunApp(
    std::shared_ptr<BABYLON::IRenderableScene> initialScene,
    const SceneWithInspectorOptions & options
  )
  {
    _appContext._options = options;

    std::function<bool(void)> showGuiLambda = [this]() -> bool {
      bool r = this->render();
      for (auto f : _appContext._options._heartbeatCallbacks)
        f();
      if (_appContext._options._sandboxCompilerCallback)
      {
        SandboxCompilerStatus sandboxCompilerStatus = _appContext._options._sandboxCompilerCallback();
        if (sandboxCompilerStatus._renderableScene)
          setRenderableScene(sandboxCompilerStatus._renderableScene);
        _appContext._isCompiling = sandboxCompilerStatus._isCompiling;
      }

      return r;
    };
    auto initSceneLambda = [&]() {
      this->initScene();
      this->setRenderableScene(initialScene);
    };
    ImGuiUtils::ImGuiRunner::RunGui(showGuiLambda, _appContext._options._appWindowParams, initSceneLambda);
  }


private:
  enum class ViewState
  {
    Scene3d,
    SamplesCodeViewer,
    SampleBrowser,
#ifdef BABYLON_BUILD_SANDBOX
    SandboxEditor,
#endif
  };
  static std::map<BabylonInspectorApp::ViewState, std::string> ViewStateLabels;

  void initScene()
  {
    _appContext._sampleListComponent.OnNewRenderableScene = [&](std::shared_ptr<IRenderableScene> scene) {
      this->setRenderableScene(scene);
    };
    _appContext._sampleListComponent.OnEditFiles = [&](const std::vector<std::string> & files) {
      _samplesCodeEditor.setFiles(files);
      _appContext._viewState = ViewState::SamplesCodeViewer;
    };
    _appContext._sampleListComponent.OnLoopSamples = [&](const std::vector<std::string> & samples) {
      _appContext._loopSamples.flagLoop = true;
      _appContext._loopSamples.samplesToLoop = samples;
      _appContext._loopSamples.currentIdx = 0;
      _appContext._viewState = ViewState::Scene3d;
    };

    _appContext._sceneWidget = std::make_unique<BABYLON::ImGuiSceneWidget>(getSceneSize());
    _appContext._sceneWidget->OnBeforeResize.push_back(
      [this]() {
        _appContext._inspector.release();
    }
    );
  }

  ImVec2 getSceneSize()
  {
    ImVec2 sceneSize = ImGui::GetIO().DisplaySize;
    sceneSize.x -= INSPECTOR_WIDTH;
    sceneSize.y -= 60;
    return sceneSize;
  }
  ImVec2 getSceneSizeSmall()
  {
    ImVec2 sceneSize = getSceneSize();
    sceneSize.x /= 3.f;
    sceneSize.y /= 3.f;
    return sceneSize;
  }

  void createInspectorIfNeeded()
  {
    auto currentScene = _appContext._sceneWidget->getScene();
    if ((! _appContext._inspector) || (_appContext._inspector->scene() != currentScene))
    {
      _appContext._inspector = std::make_unique<BABYLON::Inspector>(nullptr, _appContext._sceneWidget->getScene());
      _appContext._inspector->setScene(currentScene);
    }
  }

  bool render() // renders the GUI. Returns true when exit required
  {

    bool shallExit = false;
    createInspectorIfNeeded();
    _appContext._inspector->render(false, INSPECTOR_WIDTH);
    ImGui::SameLine();

    ImGui::BeginGroup();
    ImGui::Text("%s", _appContext._sceneWidget->getRenderableScene()->getName());
    ImGui::SameLine(0., 100.);

    ShowTabBarEnum(ViewStateLabels, &_appContext._viewState);

    ImGui::SameLine(0.f, 80.f);
    BABYLON::BabylonLogsWindow::instance().render();
    ImGui::SameLine();
    if (ImGui::Button(ICON_FA_DOOR_OPEN  " Exit"))
      shallExit = true;

    ImGui::Separator();

    if (_appContext._viewState == ViewState::Scene3d)
      _appContext._sceneWidget->render(getSceneSize());
    if (_appContext._viewState == ViewState::SamplesCodeViewer)
      _samplesCodeEditor.render();
    else if (_appContext._viewState == ViewState::SampleBrowser)
      _appContext._sampleListComponent.render();
#ifdef BABYLON_BUILD_SANDBOX
    if (_appContext._viewState == ViewState::SandboxEditor)
      renderSandbox();
#endif

    ImGui::EndGroup();

    handleLoopSamples();

    if (_appContext._options._flagScreenshotOneSampleAndExit)
      return saveScreenshot();
    else
      return shallExit;
  }

  void setRenderableScene(std::shared_ptr<BABYLON::IRenderableScene> scene)
  {
    if (_appContext._inspector)
      _appContext._inspector->setScene(nullptr);
    _appContext._sceneWidget->setRenderableScene(scene);
    if (_appContext._inspector)
      _appContext._inspector->setScene(_appContext._sceneWidget->getScene());
    if (_appContext._viewState == ViewState::SampleBrowser)
      _appContext._viewState = ViewState::Scene3d;
  }

  // Saves a screenshot after  few frames (eeturns true when done)
  bool saveScreenshot()
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

  void renderSandbox()
  {
    ImGui::BeginGroup();
    ImGui::Text("Sandbox : you can edit the code below!");
    ImGui::Text("As soon as you save it, the code will be compiled and the 3D scene will be updated");
    _appContext._sceneWidget->render(getSceneSizeSmall());

    // Render IRenderableSceneWithHud->hudGui when available
    auto asSceneWithHud = dynamic_cast<IRenderableSceneWithHud*>(
      _appContext._sceneWidget->getRenderableScene());
    if ( (asSceneWithHud) && (asSceneWithHud->hudGui)) {
      ImGui::SameLine();
      if (ImGui::Button("Hud"))
        ImGui::OpenPopup("Hud");
      if (ImGui::BeginPopup("Hud")) {
        asSceneWithHud->hudGui();
        ImGui::EndPopup();
      }
    }

    ImGui::EndGroup();

    if (_appContext._isCompiling) {
      ImGui::TextColored(ImVec4(1., 0., 0., 1.), "Compiling");
      BabylonLogsWindow::instance().setVisible(true);
    }
    if (ImGui::Button(ICON_FA_PLAY " Run"))
      _sandboxCodeEditor.saveAll();

    _sandboxCodeEditor.render();

  }


private:
  void handleLoopSamples()
  {
    static BABYLON::Samples::SamplesIndex samplesIndex;
    if (!_appContext._loopSamples.flagLoop)
      return;

    static int frame_counter = 0;
    const int max_frames = 60;

    if (frame_counter > max_frames)
    {
      std::string sampleName = _appContext._loopSamples.samplesToLoop[_appContext._loopSamples.currentIdx];
      BABYLON_LOG_ERROR("LoopSample", sampleName)
      auto scene = samplesIndex.createRenderableScene(sampleName, nullptr);
      this->setRenderableScene(scene);

      if (_appContext._loopSamples.currentIdx < _appContext._loopSamples.samplesToLoop.size() - 2)
        _appContext._loopSamples.currentIdx++;
      else
        _appContext._loopSamples.flagLoop = false;

      frame_counter = 0;
    }
    else
      frame_counter++;
  }


  struct AppContext
  {
    std::unique_ptr<BABYLON::ImGuiSceneWidget> _sceneWidget;
    std::unique_ptr< BABYLON::Inspector> _inspector;
    BABYLON::SamplesBrowser _sampleListComponent;
    ViewState _viewState = ViewState::Scene3d;
    int _frameCounter = 0;
    SceneWithInspectorOptions _options;
    bool _isCompiling = false;

    struct
    {
      bool flagLoop = false;
      size_t currentIdx = 0;
      std::vector<std::string> samplesToLoop;
    } _loopSamples;
  };

  AppContext _appContext;
  ImGuiUtils::CodeEditor _samplesCodeEditor = ImGuiUtils::CodeEditor(true); // true <-> showCheckboxReadOnly
  ImGuiUtils::CodeEditor _sandboxCodeEditor;
}; // end of class BabylonInspectorApp


std::map<BabylonInspectorApp::ViewState, std::string> BabylonInspectorApp::ViewStateLabels = {
{ BabylonInspectorApp::ViewState::Scene3d, ICON_FA_CUBE " 3D Scene"},
{ BabylonInspectorApp::ViewState::SampleBrowser, ICON_FA_PALETTE " Browse samples"},
{ BabylonInspectorApp::ViewState::SamplesCodeViewer, ICON_FA_EDIT " Samples Code Viewer"},
#ifdef BABYLON_BUILD_SANDBOX
{ BabylonInspectorApp::ViewState::SandboxEditor, ICON_FA_FLASK " Sandbox"},
#endif
};


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
