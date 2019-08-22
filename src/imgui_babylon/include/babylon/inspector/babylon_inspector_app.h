#ifndef BABYLON_INSPECTOR_APP_H
#define BABYLON_INSPECTOR_APP_H
#include <map>
#include <babylon/imgui_babylon/scene_widget_Imgui.h>
#include <babylon/inspector/inspector.h>
#include <babylon/imgui_babylon/sample_list_page.h>

// Note : the api could be vastly simplified by hiding this class inside the cpp file
// Only one function would be provided:
// void BabylonInspectorApp(std::shared_ptr<BABYLON::IRenderableScene> initialScene = nullptr);

namespace BABYLON {

  class BabylonInspectorApp {
  public:

    BabylonInspectorApp();
    void RunApp(
      std::shared_ptr<BABYLON::IRenderableScene> initialScene = nullptr, 
      std::string sceneName = "",
      bool screenshotAndExit = false
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
      std::unique_ptr<BABYLON::SceneWidget_ImGui> _sceneWidget;
      BABYLON::Inspector _inspector;
      BABYLON::SampleListPage _sampleListComponent;
      ViewState _viewState = ViewState::Scene3d;
      std::string _sceneName = "";
      bool _screenshotAndExit;
      int _frameCounter = 0;
    };

    AppContext _appContext;
  }; // end of class BabylonInspectorApp

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_APP_H
