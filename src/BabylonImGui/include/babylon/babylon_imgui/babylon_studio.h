#ifndef BABYLON_INSPECTOR_APP_H
#define BABYLON_INSPECTOR_APP_H
#include <babylon/babylon_imgui/imgui_scene_widget.h>
#include <babylon/inspector/inspector.h>
#include <babylon/inspector/samples_browser.h>
#include <functional>
#include <imgui_utils/imgui_runner_babylon/runner_babylon.h>
#include <imgui_utils/code_editor.h>
#include <babylon/babylon_api.h>
#include <map>


namespace BABYLON {
  const std::string screenshotsDirectory();

  struct BABYLON_SHARED_EXPORT PlaygroundCompilerStatus
  {
    std::shared_ptr<IRenderableScene> _renderableScene = nullptr;
    bool _isCompiling = false;
  };

  struct BABYLON_SHARED_EXPORT BabylonStudioOptions
  {
    inline BabylonStudioOptions() {
      _appWindowParams.Title = "BabylonCpp";
      _appWindowParams.DefaultWindowType = ImGuiUtils::ImGuiRunner::DefaultWindowTypeOption::ProvideFullScreenDockSpace;
    }
    std::string _sceneName = "";
    bool _flagScreenshotOneSampleAndExit = false;
    ImGuiUtils::ImGuiRunner::AppWindowParams _appWindowParams;
    
    // If defined, this function will be called at each frame
    // Place your callback here.
    using HeartbeatCallback = std::function<void(void)>;
    std::vector<HeartbeatCallback> _heartbeatCallbacks;

    // this callback is used by  the playground compiler
    using PlaygroundCompilerCallback = std::function<PlaygroundCompilerStatus(void)>;
    PlaygroundCompilerCallback _playgroundCompilerCallback;
  };

  void BABYLON_SHARED_EXPORT runBabylonStudio(
    const std::shared_ptr<BABYLON::IRenderableScene>& scene = nullptr,
    BabylonStudioOptions options = BabylonStudioOptions()
  );
} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_APP_H
