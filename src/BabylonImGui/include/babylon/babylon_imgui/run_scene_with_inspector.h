#ifndef BABYLON_INSPECTOR_APP_H
#define BABYLON_INSPECTOR_APP_H
#include <map>
#include <babylon/babylon_imgui/scene_widget_Imgui.h>
#include <babylon/inspector/inspector.h>
#include <babylon/inspector/samples_browser.h>
#include <imgui_utils/code_editor.h>
#include <imgui_utils/app_runner/imgui_runner.h>

namespace BABYLON {

  struct SceneWithInspectorOptions
  {
    inline SceneWithInspectorOptions() {
      _appWindowParams.Title = "BabylonCpp inspector";
    }
    std::string _sceneName = "";
    bool _flagScreenshotOneSampleAndExit = false;
    ImGuiUtils::ImGuiRunner::AppWindowParams _appWindowParams;
  };

  void runSceneWithInspector(
    std::shared_ptr<BABYLON::IRenderableScene> scene,
    SceneWithInspectorOptions options = SceneWithInspectorOptions()
  );
} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_APP_H
