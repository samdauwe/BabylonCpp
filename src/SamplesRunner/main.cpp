#include <babylon/inspector/babylon_inspector_app.h>
#include "SamplesRunner/HelloScene.h"
#include <babylon/samples/samples_index.h>
#include <babylon/core/logging.h>
#include <iostream>
#include "console_logger.h"
#include "chdir_to_exe.h"
#include "CLI11.h"

#ifdef _WIN32
#include <process.h>
#endif

static BABYLON::Samples::SamplesIndex samplesIndex;

void spawnScreenshotAllSamples(std::string exeName) 
{
  auto samples = samplesIndex.getSampleNames();
  for (auto sample : samples)
  {
    BABYLON_LOG_INFO("ScreenshotAllSamples", sample);
    auto does_fail = samplesIndex.doesSampleFail(sample);
    if (does_fail) {
      BABYLON_LOG_INFO("ScreenshotAllSamples", "Skipping failing ", sample, ": ", SampleFailureReason_Str(does_fail.value()) );
    }
    else
      _spawnl(P_WAIT, exeName.c_str(), exeName.c_str(), "-s", sample.c_str(), "-p", NULL);
  }
}

int main(int argc, char** argv)
{
  ChdirToExe(argv[0]);

  bool verbose = false;
  bool screenshotAllSamples = false;
  bool screenshotOneSampleAndExit = false;
  bool listSamples = false;
  std::string sampleName = "";
  std::string sampleGroup = "";
  {
    CLI::App arg_cli{ "BabylonCpp samples runner" };
    arg_cli.add_flag("-v,--verbose", verbose, "Verbose mode");
    arg_cli.add_option("-s,--sample", sampleName, "Which sample to run");
    arg_cli.add_option("-g,--sample-group", sampleGroup, "Which sample group to run");
    arg_cli.add_flag("-l,--list", listSamples, "List samples");
    arg_cli.add_flag("-a,--shot-all-samples", screenshotAllSamples, "run all samples and save a screenshot");
    arg_cli.add_flag("-p,--shot-one-sample", screenshotOneSampleAndExit, "run one sample, save a screenshot and exit");
    CLI11_PARSE(arg_cli, argc, argv);
  }

  if (verbose)
    BABYLON::impl::initializeConsoleLogger();

  if (screenshotAllSamples) {
    spawnScreenshotAllSamples(argv[0]); exit(0);
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
  app.RunApp(scene, sampleName, screenshotOneSampleAndExit);
  return 0;
}
