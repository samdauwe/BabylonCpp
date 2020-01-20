#include <babylon/samples/samples_index.h>
#include <babylon/core/logging.h>
#include <iostream>
#include <string>
#include <future>
#include <atomic>
#include <chrono>
#include "subprocess_cpp_wrapper.h"
#include <babylon/asio/internal/future_utils.h>
#include <babylon/core/filesystem.h>
#include <babylon/core/timer.h>
#include <babylon/babylon_imgui/babylon_studio.h>

#ifdef __linux__
#include <sys/types.h>
#include <signal.h>
#endif

#ifdef _MSC_VER
#pragma warning(disable: 4996)
#endif

namespace BABYLON {
namespace impl {

struct SpawnOptions {
  bool        InheritEnvironment = true;
  bool        MixStdOutStdErr = true;
  double      MaxExecutionTimeSeconds = -1;
  bool        CopyOutputToMainProgramOutput  = false;
};

struct SpawnResult
{
  int ExitStatus = -99;
  bool MaxExecutionTimePassed = false;
  std::string StdOutErr = "";
};

SpawnResult SpawnWaitSubProcess(
  const std::vector<std::string> & command,
  const SpawnOptions&spawnOptions
  )
{
  SpawnResult spawnResult;

  using namespace std::literals;

  struct subprocess_s subProcess;

  int processOptions = 0;
  if (spawnOptions.MixStdOutStdErr)
    processOptions = processOptions | subprocess_option_combined_stdout_stderr;
  if (spawnOptions.InheritEnvironment)
    processOptions = processOptions | subprocess_option_inherit_environment;


  int resultCreateProcess = subprocess_create_cpp(
    command, subprocess_option_inherit_environment | subprocess_option_combined_stdout_stderr,
    &subProcess);
  if (0 != resultCreateProcess)
    throw std::runtime_error("spawnScreenshots: Error while spawning screenshot process");
  Timer timer;
  timer.start();

  FILE* outPipe    = subprocess_stdout(&subProcess);
  std::atomic<bool> wasProcessKilled = false;

  auto lambdaCopyOutput = [outPipe, spawnOptions, &spawnResult, &wasProcessKilled]() {
    char line[256];
    while ( (! wasProcessKilled) && (fgets( line, sizeof line, outPipe)))
    {
      if (spawnOptions.CopyOutputToMainProgramOutput)
        printf("%s", line);

      spawnResult.StdOutErr = spawnResult.StdOutErr + line;
    }
  };

  auto asyncCallFuture = std::async(std::launch::async, lambdaCopyOutput);

  bool done = false;
  while (!done)
  {
    std::this_thread::sleep_for(500ms);

    auto status = asio::get_future_running_status(asyncCallFuture);
    if (status == asio::future_running_status::future_ready)
      done = true;

    double elapsedSeconds = timer.getElapsedTimeInSec();
    if ((spawnOptions.MaxExecutionTimeSeconds > 0.) && (elapsedSeconds > spawnOptions.MaxExecutionTimeSeconds))
    {
      wasProcessKilled = true;
#ifdef __linux__
      int resultKillSubprocess = kill(subProcess.child, 9);
#else
      int resultKillSubprocess = subprocess_destroy(&subProcess);
#endif
      if (0 != resultKillSubprocess)
        throw std::runtime_error("spawnScreenshots: Error while killing subprocess after timeeout");
      done = true;
      spawnResult.MaxExecutionTimePassed = true;
    }
  }

  fclose(outPipe);
  subprocess_join(&subProcess, &spawnResult.ExitStatus);
  return spawnResult;
}


void spawnScreenshots(const std::string & exeName, bool skipFailing)
{
#ifdef _WIN32
  BABYLON_LOG_ERROR("spawnScreenshots", " needs fix under windows...");
  return;
#endif
  if (!BABYLON::Filesystem::isDirectory(screenshotsDirectory()))
    BABYLON::Filesystem::createDirectory(screenshotsDirectory());

  BABYLON::Samples::SamplesIndex& samplesIndex = Samples::SamplesIndex::Instance();
  auto samples = samplesIndex.getSampleNames();
  for (auto sample : samples)
  {
    // sample = "ImportBabylonJSLogoScene";
    BABYLON_LOG_INFO("ScreenshotAllSamples", sample);
    auto does_fail = samplesIndex.doesSampleFail(sample);
    if (does_fail && skipFailing) {
      BABYLON_LOG_INFO("ScreenshotAllSamples", "Skipping failing ", sample, ": ", SampleFailureReason_Str(does_fail.value().Kind));
    }
    else
    {
      std::vector<std::string> command =  { exeName, "-s", sample, "-p"};
      SpawnOptions spawnOptions;
      spawnOptions.MaxExecutionTimeSeconds = 5.;
      spawnOptions.CopyOutputToMainProgramOutput = false;
      auto spawnResult = SpawnWaitSubProcess(command, spawnOptions);

      if (spawnResult.ExitStatus != 0)
      {
        BABYLON_LOG_WARN("ScreenshotAllSamples", "Subprocess has failed for sample ", sample);
        std::string outputFilename = screenshotsDirectory() + "/" + sample + ".Fail.txt";
        FILE* f                    = fopen(outputFilename.c_str(), "w");
        fprintf(f, "%s", spawnResult.StdOutErr.c_str());
        fclose(f);
      }

      if (spawnResult.MaxExecutionTimePassed)
      {
        BABYLON_LOG_WARN("ScreenshotAllSamples", "Subprocess MaxExecutionTimePassed for sample ", sample);
        std::string outputFilename = screenshotsDirectory() + "/" + sample + ".TooSlow.txt";
        FILE * f = fopen(outputFilename.c_str(), "w");
        fprintf(f, "Process was killed because it took too long");
        fclose(f);
      }
    }
  }
}


} // namespace impl
} // namespace BABYLON
