#include <babylon/inspector/babylon_inspector_app.h>
#include <babylon/core/system.h>
#include "SamplesRunner/HelloScene.h"
#include <babylon/samples/samples_index.h>
#include <babylon/core/logging.h>
#include <iostream>
#include <babylon/core/logging/init_console_logger.h>
#include <babylon/utils/CLI11.h>
#include "spawn_screenshots.h"
#include <imgui_utils/code_editor.h>

int main(int argc, char** argv)
{
  BABYLON::System::chdirToExecutableFolder();

  BABYLON::Samples::SamplesIndex samplesIndex;
  bool flagVerbose = false;
  bool flagSpawnScreenshots = false;
  bool flagScreenshotOneSampleAndExit = false;
  bool listSamples = false;
  std::string sampleName = "";
  std::string sampleGroup = "";
  {
    CLI::App arg_cli{ "BabylonCpp samples runner" };
    arg_cli.add_flag("-v,--verbose", flagVerbose, "Verbose mode");
    arg_cli.add_option("-s,--sample", sampleName, "Which sample to run");
    arg_cli.add_option("-g,--sample-group", sampleGroup, "Which sample group to run");
    arg_cli.add_flag("-l,--list", listSamples, "List samples");
    arg_cli.add_flag("-a,--shot-all-samples", flagSpawnScreenshots, "run all samples and save a screenshot");
    arg_cli.add_flag("-p,--shot-one-sample", flagScreenshotOneSampleAndExit, "run one sample, save a screenshot and exit");
    CLI11_PARSE(arg_cli, argc, argv);
  }

  if (flagVerbose)
    BABYLON::initConsoleLogger();

  if (flagSpawnScreenshots) {
    bool skipFailingSamples = true;
    BABYLON::impl::spawnScreenshots(argv[0], skipFailingSamples); exit(0);
  }

  if (listSamples) {
    samplesIndex.listSamples(); exit(0);
  }

  std::shared_ptr<BABYLON::IRenderableScene> scene;
  if (!sampleName.empty())
    scene = samplesIndex.createRenderableScene(sampleName, nullptr);
  else
    scene = SamplesRunner::MakeHelloScene();

  BABYLON::BabylonInspectorApp app;
  app.RunApp(scene, sampleName, flagScreenshotOneSampleAndExit);
  return 0;
}
