#ifndef BABYLON_INSPECTOR_APP_H
#define BABYLON_INSPECTOR_APP_H
#include <functional>
#include <map>
#include <babylon/babylon_imgui/imgui_scene_widget.h>
#include <babylon/inspector/inspector.h>
#include <babylon/inspector/samples_browser.h>
#include <imgui_utils/code_editor.h>
#include <imgui_utils/app_runner/imgui_runner.h>

namespace BABYLON {

  struct PlaygroundCompilerStatus
  {
    std::shared_ptr<IRenderableScene> _renderableScene = nullptr;
    bool _isCompiling = false;
  };

  struct SceneWithInspectorOptions
  {
    inline SceneWithInspectorOptions() {
      _appWindowParams.Title = "BabylonCpp inspector";
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

  void runSceneWithInspector(
    std::shared_ptr<BABYLON::IRenderableScene> scene,
    SceneWithInspectorOptions options = SceneWithInspectorOptions()
  );
} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_APP_H
