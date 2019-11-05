#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_utils/icons_font_awesome_5.h>
#include <babylon/babylon_imgui/run_scene_with_inspector.h>
#include <babylon/babylon_imgui/babylon_logs_window.h>
#include <babylon/interfaces/irenderable_scene_with_hud.h>
#include <babylon/inspector/components/actiontabs/action_tabs_component.h>
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


class BabylonInspectorApp {
public:

  BabylonInspectorApp() {
    std::string exePath = BABYLON::System::getExecutablePath();
    std::string exeFolder = BABYLON::Filesystem::baseDir(exePath);
    std::string playgroundPath = exeFolder + "/../../../src/BabylonStudio/playground.cpp";
    playgroundPath = BABYLON::Filesystem::absolutePath(playgroundPath);
    _playgroundCodeEditor.setFiles({ playgroundPath });
    _playgroundCodeEditor.setLightPalette();
    ApplyLayoutMode(LayoutMode::SceneAndInspector);
  }

  void RunApp(
    std::shared_ptr<BABYLON::IRenderableScene> initialScene,
    const SceneWithInspectorOptions & options
  )
  {
    _appContext._options = options;
    _appContext._options._appWindowParams.WindowedFullScreen = true;

    std::function<bool(void)> showGuiLambda = [this]() -> bool
    {
      bool r = this->render();
      for (auto f : _appContext._options._heartbeatCallbacks)
        f();
      if (_appContext._options._playgroundCompilerCallback)
      {
        PlaygroundCompilerStatus playgroundCompilerStatus = _appContext._options._playgroundCompilerCallback();
        if (playgroundCompilerStatus._renderableScene)
          setRenderableScene(playgroundCompilerStatus._renderableScene);
        _appContext._isCompiling = playgroundCompilerStatus._isCompiling;
      }
      return r;
    };

    auto initSceneLambda = [&]()
    {
      this->initScene();
      this->setRenderableScene(initialScene);
    };

    //_appContext._options._appWindowParams.DefaultWindowType = ImGuiUtils::ImGuiRunner::DefaultWindowTypeOption::ProvideFullScreenWindow;
    _appContext._options._appWindowParams.InitialDockLayoutFunction = [this](ImGuiID mainDockId) {
      CreateDockingLayout_BabylonCpp(mainDockId);
    };
    ImGuiUtils::ImGuiRunner::RunGui(showGuiLambda, _appContext._options._appWindowParams, initSceneLambda);
  }


private:
  enum class ViewElements
  {
    Scene3d,
    SamplesCodeViewer,
    SampleBrowser,
#ifdef BABYLON_BUILD_PLAYGROUND
    PlaygroundEditor,
#endif
  };

  static std::map<BabylonInspectorApp::ViewElements, std::string> _viewElementsLabels;
  bool
    _showScene3d = true,
    _showSamplesBrowser = true,
    _showSamplesCodeViewer = true,
    _showPlayground = true,
    _showLogs = true,
    _showInspector = true,
    _showLayout = true;

  enum class LayoutMode { Scene, SceneAndInspector, Dev };
  LayoutMode _layoutMode = LayoutMode::SceneAndInspector;


  void CreateDockingLayout_BabylonCpp(ImGuiID fullDockSpaceId)
  {
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::DockBuilderRemoveNode(fullDockSpaceId); // Clear out existing layout
    ImGui::DockBuilderAddNode(fullDockSpaceId);    // Add empty node
    ImGui::DockBuilderSetNodeSize(fullDockSpaceId, viewport->Size);

    ImGuiID dock_main_id = fullDockSpaceId; // This variable will track the document node, however we are not using it
                                            // here as we aren't docking anything into it.
    //ImGuiID dock_id_top    = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Up, 0.10f, NULL, &dock_main_id);
    ImGuiID dock_id_bottom = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Down, 0.25f, NULL, &dock_main_id);
    ImGuiID dock_id_left   = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Left, 0.25f, NULL, &dock_main_id);
    ImGuiID dock_id_right  = ImGui::DockBuilderSplitNode(dock_main_id, ImGuiDir_Right, 0.45f, NULL, &dock_main_id);

    ImGuiID dock_id_bottom_left = 0, dock_id_left_bottom = 0;
    if (_layoutMode == LayoutMode::Dev)
      dock_id_bottom_left = ImGui::DockBuilderSplitNode(dock_id_bottom, ImGuiDir_Left, 0.20f, NULL, &dock_id_bottom);
    if (_layoutMode == LayoutMode::SceneAndInspector)
      dock_id_left_bottom = ImGui::DockBuilderSplitNode(dock_id_left, ImGuiDir_Down, 0.25f, NULL, &dock_id_left);

    ImGui::DockBuilderDockWindow("Inspector", dock_id_left);

#ifdef BABYLON_BUILD_PLAYGROUND
    ImGui::DockBuilderDockWindow(_viewElementsLabels.at(ViewElements::PlaygroundEditor).c_str(), dock_main_id);
#endif
    ImGui::DockBuilderDockWindow(_viewElementsLabels.at(ViewElements::SamplesCodeViewer).c_str(), dock_main_id);
    ImGui::DockBuilderDockWindow(_viewElementsLabels.at(ViewElements::SampleBrowser).c_str(), dock_id_left);
    ImGui::DockBuilderDockWindow(_viewElementsLabels.at(ViewElements::Scene3d).c_str(), dock_id_right);
    ImGui::DockBuilderDockWindow("Logs", dock_id_bottom);
    if (_layoutMode == LayoutMode::Dev)
      ImGui::DockBuilderDockWindow("Layout", dock_id_bottom_left);
    if (_layoutMode == LayoutMode::SceneAndInspector)
      ImGui::DockBuilderDockWindow("Layout", dock_id_left_bottom);

    ImGui::DockBuilderFinish(fullDockSpaceId);
  }


  void initScene()
  {
    _appContext._sampleListComponent.OnNewRenderableScene = [&](std::shared_ptr<IRenderableScene> scene)
    {
      this->setRenderableScene(scene);

      // Focus 3d widget when changing scene
      auto winLabel = _viewElementsLabels.at(ViewElements::Scene3d).c_str();
      ImGuiWindow* sceneWindow = ImGui::FindWindowByName(winLabel);
      ImGui::FocusWindow(sceneWindow);
    };

    _appContext._sampleListComponent.OnEditFiles = [&](const std::vector<std::string> & files) {
      _samplesCodeEditor.setFiles(files);

      _showSamplesCodeViewer = true;
      // Focus code editor
      auto winLabel            = _viewElementsLabels.at(ViewElements::SamplesCodeViewer).c_str();
      ImGuiWindow* codeWindow = ImGui::FindWindowByName(winLabel);
      ImGui::FocusWindow(codeWindow);
    };
    _appContext._sampleListComponent.OnLoopSamples = [&](const std::vector<std::string> & samples) {
      _appContext._loopSamples.flagLoop = true;
      _appContext._loopSamples.samplesToLoop = samples;
      _appContext._loopSamples.currentIdx = 0;
    };

    _appContext._sceneWidget = std::make_unique<BABYLON::ImGuiSceneWidget>(ImVec2(640.f, 480.f));
    _appContext._sceneWidget->OnBeforeResize.push_back(
      [this]() {
        _appContext._inspector.release();
    }
    );
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
    createInspectorIfNeeded();

    if (_showInspector)
      _appContext._inspector->render();

    if (_showLayout)
      GuiLayout();

    if (_showLogs)
      BABYLON::BabylonLogsWindow::instance().render();

    if (_showScene3d)
    {
      if (ImGui::Begin(_viewElementsLabels.at(ViewElements::Scene3d).c_str()))
        render3d();
      ImGui::End();
    }

    if (_showSamplesCodeViewer)
    {
      if (!_samplesCodeEditor.isEmpty())
      {
        const char* title = _viewElementsLabels.at(ViewElements::SamplesCodeViewer).c_str();
        if (ImGui::Begin(title, &_showSamplesCodeViewer))
          _samplesCodeEditor.render();
        ImGui::End();
      }
    }

#ifdef BABYLON_BUILD_PLAYGROUND
    if (_showPlayground) {
      if (ImGui::Begin(_viewElementsLabels.at(ViewElements::PlaygroundEditor).c_str()))
        renderPlayground();
      ImGui::End();
    }
#endif

    if (_showSamplesBrowser)
    {
      if (ImGui::Begin(_viewElementsLabels.at(ViewElements::SampleBrowser).c_str()))
        _appContext._sampleListComponent.render();
      ImGui::End();
    }


    handleLoopSamples();

    if (_appContext._options._flagScreenshotOneSampleAndExit)
      return saveScreenshot();
    else
      return false;
  }

  void GuiLayout()
  {
    int windowFlags = 0;

    if (_layoutMode == LayoutMode::Scene)
    {
      ImVec2 winPosition(0.f, ImGui::GetIO().DisplaySize.y - 200.f);
      ImGui::SetNextWindowBgAlpha(0.5f);
      windowFlags = windowFlags | ImGuiWindowFlags_NoDocking;
    }
    ImGui::Begin("Layout", nullptr, windowFlags);
    GuiLayoutImpl();
    ImGui::End();
  }

  void GuiLayoutImpl()
  {
    if (ImGui::RadioButton(ICON_FA_CUBE " Scene", _layoutMode == LayoutMode::Scene))
      ApplyLayoutMode(LayoutMode::Scene);
    if (ImGui::RadioButton(ICON_FA_CUBES " Scene, Inspector and Samples", _layoutMode == LayoutMode::SceneAndInspector))
      ApplyLayoutMode(LayoutMode::SceneAndInspector);
    if (ImGui::RadioButton(ICON_FA_FILE_CODE " Dev", _layoutMode == LayoutMode::Dev))
      ApplyLayoutMode(LayoutMode::Dev);
  }

  void ApplyLayoutMode(LayoutMode mode)
  {
    _layoutMode = mode;
    switch (_layoutMode)
    {
    case LayoutMode::Scene:
        _showScene3d           = true;
        _showSamplesBrowser    = false;
        _showInspector         = false;
        _showSamplesCodeViewer = false;
        _showPlayground        = false;
        _showLogs              = false;
        _showLayout            = false;
      break;
    case LayoutMode::SceneAndInspector:
      _showScene3d           = true;
      _showSamplesBrowser    = true;
      _showInspector         = true;
      _showSamplesCodeViewer = false;
      _showPlayground        = false;
      _showLogs              = false;
      _showLayout            = true;
      break;
    case LayoutMode::Dev:
      _showScene3d           = true;
      _showSamplesBrowser    = false;
      _showSamplesCodeViewer = true;
      _showPlayground        = true;
      _showLogs              = true;
      _showInspector         = false;
      _showLayout            = true;
      break;
    default:
      break;
    }
    ImGuiUtils::ImGuiRunner::ResetDockLayout();
  }


  void setRenderableScene(std::shared_ptr<BABYLON::IRenderableScene> scene)
  {
    if (_appContext._inspector)
      _appContext._inspector->setScene(nullptr);
    _appContext._sceneWidget->setRenderableScene(scene);
    if (_appContext._inspector)
      _appContext._inspector->setScene(_appContext._sceneWidget->getScene());
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

  bool ButtonInOverlayWindow(const std::string& label, ImVec2 position, ImVec2 size)
  {
    ImGui::SetNextWindowPos(position);
    ImGui::SetNextWindowSize(size);
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground;
    std::string id = label + "##ButtonInOverlayWindow";
    ImGui::Begin(label.c_str(), nullptr, flags);
    bool clicked = ImGui::Button(label.c_str());
    ImGui::End();
    return clicked;
  }

  void renderHud(ImVec2 cursorScene3dTopLeft)
  {
    auto asSceneWithHud = dynamic_cast<IRenderableSceneWithHud*>(
      _appContext._sceneWidget->getRenderableScene());
    if (!asSceneWithHud)
      return;
    if (!asSceneWithHud->hudGui)
      return;

    static bool showHud = false;

    ImVec2 hudButtonPosition(cursorScene3dTopLeft.x + 2.f, cursorScene3dTopLeft.y + 2.f);
    ImVec2 hudWindowPosition(cursorScene3dTopLeft.x + 2.f, cursorScene3dTopLeft.y + 30.f);
    // ImVec2 hudWindowSize(scene3dSize.x / 2.f, scene3dSize.y / 2.f);

    if (ButtonInOverlayWindow(ICON_FA_COG, hudButtonPosition, ImVec2(30.f, 30.f)))
      showHud = !showHud;

    if (showHud) {
      ImGui::SetNextWindowPos(hudWindowPosition, ImGuiCond_Once);
      //ImGui::SetNextWindowSize(hudWindowSize, ImGuiCond_Once);
      ImGui::SetNextWindowBgAlpha(0.5f);
      ImGuiWindowFlags flags = ImGuiWindowFlags_AlwaysAutoResize;
      ImGui::Begin("HUD", &showHud, flags);
      asSceneWithHud->hudGui();
      ImGui::End();
    }
  }

  void render3d()
  {
    ImGui::Text("%s", _appContext._sceneWidget->getRenderableScene()->getName());
    if (!_showLayout)
    {
      GuiLayout();
    }
    ImVec2 sceneSize = ImGui::GetCurrentWindow()->Size;
    sceneSize.y -= 55.f;

    ImVec2 cursorPosBeforeScene3d = ImGui::GetCursorScreenPos();
    _appContext._sceneWidget->render(sceneSize);
    renderHud(cursorPosBeforeScene3d);
  }

  void renderPlayground()
  {
    ImGui::Button(ICON_FA_QUESTION_CIRCLE);
    if (ImGui::IsItemHovered())
      ImGui::SetTooltip("Playground : you can edit the code below! As soon as you save it, the code will be compiled and the 3D scene will be updated");
    ImGui::SameLine();

    if (_appContext._isCompiling) {
      ImGui::TextColored(ImVec4(1., 0., 0., 1.), "Compiling");
      BabylonLogsWindow::instance().setVisible(true);
    }
    if (ImGui::Button(ICON_FA_PLAY " Run"))
      _playgroundCodeEditor.saveAll();
    ImGui::SameLine();
    _playgroundCodeEditor.render();
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
  ImGuiUtils::CodeEditor _playgroundCodeEditor;
}; // end of class BabylonInspectorApp


std::map<BabylonInspectorApp::ViewElements, std::string> BabylonInspectorApp::_viewElementsLabels = {
{ BabylonInspectorApp::ViewElements::Scene3d, ICON_FA_CUBE " 3D Scene"},
{ BabylonInspectorApp::ViewElements::SampleBrowser, ICON_FA_PALETTE " Browse samples"},
{ BabylonInspectorApp::ViewElements::SamplesCodeViewer, ICON_FA_EDIT " Samples Code Viewer"},
#ifdef BABYLON_BUILD_PLAYGROUND
{ BabylonInspectorApp::ViewElements::PlaygroundEditor, ICON_FA_FLASK " Playground"},
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
