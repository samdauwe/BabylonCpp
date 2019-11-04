#include <babylon/core/system.h>
#include "SamplesRunner/HelloScene.h"
#include <babylon/samples/samples_index.h>
#include <babylon/babylon_imgui/run_scene_with_inspector.h>
#include <babylon/core/logging/init_console_logger.h>
#include <babylon/utils/CLI11.h>

#include "spawn_screenshots.h"
#include "run_standalone_imgui.h"
#include "imgui_utils/app_runner/imgui_runner_demo.h"

#ifdef BABYLON_BUILD_PLAYGROUND
#include <SamplesRunner/rtc/rtc_manager.h>
#endif

int main(int argc, char** argv)
{
#ifdef BABYLON_BUILD_PLAYGROUND
  BABYLON::rtc::RtcManager runtimeCompiler;
  runtimeCompiler.Init();
#endif

  BABYLON::System::chdirToExecutableFolder();

  ImGuiUtils::ImGuiRunner::ShowDemo();
  return 0;


  bool flagQuiet = false;
  bool flagFullscreen = false;
  bool flagSpawnScreenshots = false;
  bool flagScreenshotOneSampleAndExit = false;
  bool listSamples = false;
  bool flagStandaloneImgui = false;
  std::string sampleName = "Hello Scene";
  {
    CLI::App arg_cli{ "BabylonCpp samples runner" };
    arg_cli.add_flag("-q,--quiet", flagQuiet, "Quiet mode (not verbose)");
    arg_cli.add_option("-s,--sample", sampleName, "Which sample to run");
    arg_cli.add_flag("-l,--list", listSamples, "List samples");
    arg_cli.add_flag("-f,--fullscreen", flagFullscreen, "run in fullscreen");
    arg_cli.add_flag("-i,--standalone-imgui", flagStandaloneImgui, "run standalone scene in imgui, without the inspector");
    arg_cli.add_flag("-a,--shot-all-samples", flagSpawnScreenshots, "run all samples and save a screenshot");
    arg_cli.add_flag("-p,--shot-one-sample", flagScreenshotOneSampleAndExit, "run one sample, save a screenshot and exit");
    CLI11_PARSE(arg_cli, argc, argv);
  }

  if (!flagQuiet)
    BABYLON::initConsoleLogger();

  if (flagSpawnScreenshots) {
    bool skipFailingSamples = false;
    BABYLON::impl::spawnScreenshots(argv[0], skipFailingSamples); exit(0);
  }

  if (listSamples) {
    BABYLON::Samples::SamplesIndex().listSamples();
    exit(0);
  }

  std::shared_ptr<BABYLON::IRenderableScene> scene;
  if (sampleName == "Hello Scene")
    scene = MakeHelloScene();
  else
    scene = BABYLON::Samples::SamplesIndex().createRenderableScene(sampleName, nullptr);

  if (flagStandaloneImgui)
    runStandalone_imgui(scene, sampleName, flagFullscreen);
  else
  {
    BABYLON::SceneWithInspectorOptions options;
    options._flagScreenshotOneSampleAndExit = flagScreenshotOneSampleAndExit;
    options._sceneName = sampleName;
    options._appWindowParams.FullScreen = flagFullscreen;

#ifdef BABYLON_BUILD_PLAYGROUND
    options._playgroundCompilerCallback = [&runtimeCompiler]() {
      auto compilerStatus = runtimeCompiler.Heartbeat();
      return compilerStatus;
    };
#endif

    runSceneWithInspector(scene, options);
  }
  return 0;
}
