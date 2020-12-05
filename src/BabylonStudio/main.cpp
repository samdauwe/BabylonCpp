#include "BabylonStudio/screenshoter/spawn_screenshots.h"
#include <babylon/babylon_imgui/babylon_studio.h>
#include <babylon/core/logging.h>
#include <babylon/core/logging/init_console_logger.h>
#include <babylon/core/system.h>
#include <babylon/samples/samples_info.h>
#include <babylon/utils/CLI11.h>

#ifdef BABYLON_BUILD_PLAYGROUND
#include <BabylonStudio/rtc/rtc_manager.h>
#endif

int main(int argc, char** argv)
{
#ifdef BABYLON_BUILD_PLAYGROUND
  BABYLON::rtc::RtcManager runtimeCompiler;
  runtimeCompiler.Init();
#endif

  BABYLON::System::chdirToExecutableFolder();

  // ImGuiUtils::ImGuiRunner::ShowDemo(); return 0;

  bool flagQuiet                      = false;
  bool flagFullscreen                 = false;
  bool flagAsync                      = false;
  bool flagSpawnScreenshots           = false;
  bool flagScreenshotOneSampleAndExit = false;
  std::string sampleName;
  {
    CLI::App arg_cli{"BabylonCpp samples runner"};
    arg_cli.add_flag("-q,--quiet", flagQuiet, "Quiet mode (not verbose)");
    arg_cli.add_option("-s,--sample", sampleName, "Which sample to run");
    arg_cli.add_flag("-f,--fullscreen", flagFullscreen, "run in fullscreen");

    arg_cli.add_flag("-a,--shot-all-samples", flagSpawnScreenshots,
                     "run all samples and save a screenshot");
    arg_cli.add_flag("-p,--shot-one-sample", flagScreenshotOneSampleAndExit,
                     "run one sample, save a screenshot and exit");
    CLI11_PARSE(arg_cli, argc, argv);
  }

  if (!flagQuiet)
    BABYLON::initConsoleLogger();

  if (flagSpawnScreenshots) {
    BABYLON::impl::spawnScreenshots(argv[0], flagAsync);
    exit(0);
  }

  std::shared_ptr<BABYLON::IRenderableScene> scene;
  if (!sampleName.empty())
    scene = BABYLON::SamplesInfo::SamplesCollection::Instance().createRenderableScene(sampleName,
                                                                                      nullptr);

  BABYLON::BabylonStudioOptions options;
  options._flagScreenshotOneSampleAndExit = flagScreenshotOneSampleAndExit;
  options._sceneName                      = sampleName;
  options._appWindowParams.FullScreen     = flagFullscreen;

#ifdef BABYLON_BUILD_PLAYGROUND
  options._playgroundCompilerCallback = [&runtimeCompiler]() {
    auto compilerStatus = runtimeCompiler.Heartbeat();
    return compilerStatus;
  };
#endif

  runBabylonStudio(scene, options);
  return 0;
}
