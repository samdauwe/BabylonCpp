#include <babylon/samples/samples_index.h>
#include <babylon/core/logging.h>
#include <iostream>
#include <string>
#include <future>
#include <atomic>
#include <chrono>
#include "subprocess_cpp_wrapper.h"
#include <stb_image/stb_image.h>
#include <babylon/asio/internal/future_utils.h>
#include <babylon/core/filesystem.h>
#include <babylon/core/timer.h>
#include <babylon/babylon_imgui/babylon_studio.h>
#include <nlohmann/json.hpp>

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

bool IsImageAllTheSameColor(const std::string &imageFileName)
{
  int w,h,n;
  int force_4_channels = 4;
  unsigned char *data = stbi_load(imageFileName.c_str(), &w, &h, &n, force_4_channels);
  if (data == NULL)
    return true;

  auto are_pixels_4_channels_equal = [](const unsigned char *v1, const unsigned char *v2) {
    for (int i = 0; i < 4; i++)
      if (v1[i] != v2[i])
        return false;
    return true;
  };

  bool are_all_pixels_equal    = true;
  unsigned char * firstPixel = data;
  unsigned char * currentPixel = data;
  for (int y = 0; y < h; ++y) {
    for (int x = 0; x < w; ++x) {
      if (!are_pixels_4_channels_equal(firstPixel, currentPixel)) {
        are_all_pixels_equal = false;
      }
      currentPixel += 4;
    }
  }
  stbi_image_free(data);
  return are_all_pixels_equal;
}

bool Is3DRenderingEmpty(const std::string & sampleName)
{
  std::string filename = BABYLON::screenshotsDirectory() + "/" + sampleName + ".jpg";
  return IsImageAllTheSameColor(filename);
}

using SpawnScreenshotsStats = std::unordered_map<std::string, unsigned int>;

SpawnScreenshotsStats MakeEmptySpawnScreenshotsStats()
{
  SpawnScreenshotsStats r;
  r["nbSuccess"] = 0;
  r["nbProcessFailures"] = 0;
  r["nbProcessHung"] = 0;
  r["nbEmptyRendering"] = 0;
  return r;
}


void spawnScreenshots(const std::string & exeName)
{
#ifdef _WIN32
  BABYLON_LOG_ERROR("spawnScreenshots", " needs fix under windows...");
  return;
#endif

  auto spawnScreenshotsStats = MakeEmptySpawnScreenshotsStats();
  if (!BABYLON::Filesystem::isDirectory(screenshotsDirectory()))
    BABYLON::Filesystem::createDirectory(screenshotsDirectory());

  BABYLON::Samples::SamplesIndex& samplesIndex = Samples::SamplesIndex::Instance();
  auto samples = samplesIndex.getSampleNames();
  for (auto sampleName : samples)
  {
    BABYLON_LOG_INFO("ScreenshotAllSamples", sampleName);

    std::vector<std::string> command =  { exeName, "-s", sampleName, "-p"};
    SpawnOptions spawnOptions;
    spawnOptions.MaxExecutionTimeSeconds = 5.;
    spawnOptions.CopyOutputToMainProgramOutput = false;
    auto spawnResult = SpawnWaitSubProcess(command, spawnOptions);

    if (spawnResult.ExitStatus != 0)
    {
      BABYLON_LOG_WARN("ScreenshotAllSamples", "Subprocess has failed for sample ", sampleName);
      std::string outputFilename = screenshotsDirectory() + "/" + sampleName + ".Fail.txt";
      FILE* f                    = fopen(outputFilename.c_str(), "w");
      fprintf(f, "%s", spawnResult.StdOutErr.c_str());
      fclose(f);
      ++spawnScreenshotsStats["nbProcessFailures"];
    }

    if (spawnResult.MaxExecutionTimePassed)
    {
      BABYLON_LOG_WARN("ScreenshotAllSamples", "Subprocess MaxExecutionTimePassed for sample ",
                       sampleName);
      std::string outputFilename = screenshotsDirectory() + "/" + sampleName + ".TooSlow.txt";
      FILE * f = fopen(outputFilename.c_str(), "w");
      fprintf(f, "Process was killed because it took too long");
      fclose(f);
      ++spawnScreenshotsStats["nbProcessHung"];
    }

    if ( (spawnResult.ExitStatus == 0) && (!spawnResult.MaxExecutionTimePassed))
    {
      bool isRenderingEmpty = Is3DRenderingEmpty(sampleName);
      if (isRenderingEmpty) {
        BABYLON_LOG_WARN("ScreenshotAllSamples", "Empty 3D rendering for sample ",
                         sampleName);
        ++spawnScreenshotsStats["nbEmptyRendering"];

        std::string imageFilename = BABYLON::screenshotsDirectory() + "/" + sampleName + ".jpg";
        Filesystem::removeFile(imageFilename);

        std::string outputFilename = screenshotsDirectory() + "/" + sampleName + ".EmptyRendering.txt";
        FILE * f = fopen(outputFilename.c_str(), "w");
        fprintf(f, "3D Rendering is empty");
        fclose(f);
      }
      else
        ++spawnScreenshotsStats["nbSuccess"];
    }
  }

  // Write Stats file
  {
    nlohmann::json jsonData;
    jsonData = spawnScreenshotsStats;
    std::string outputFilename = screenshotsDirectory() + "/_stats.txt";
    FILE * f = fopen(outputFilename.c_str(), "w");
    std::string jsonString = jsonData.dump(4);
    fprintf(f, "%s", jsonString.c_str());
    fclose(f);
    BABYLON_LOG_INFO("spawnScreenshots", "stats: ", jsonData.dump());
  }
}


} // namespace impl
} // namespace BABYLON
