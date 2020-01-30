#include <babylon/babylon_common.h>
#include <babylon/core/timer.h>
#include <babylon/asio/internal/future_utils.h>
#include <stb_image/stb_image.h>
#include <babylon/samples/samples_info.h>

#include "subprocess_cpp_wrapper.h"
#include <stdexcept>
#include <atomic>
#include <future>

#include <babylon/samples/sample_spawn.h>

namespace BABYLON {
namespace Samples {

SpawnResult SpawnWaitSubProcess(
  const std::vector<std::string> & command,
  const SpawnOptions& spawnOptions
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
      int resultTerminateSubprocess = subprocess_terminate(&subProcess);
      if (0 != resultTerminateSubprocess)
        throw std::runtime_error("spawnScreenshots: Error while killing subprocess after timeeout");
      done = true;
      spawnResult.MaxExecutionTimePassed = true;
    }
  }

  fclose(outPipe);
  subprocess_join(&subProcess, &spawnResult.ExitStatus);
  return spawnResult;
}


bool ReadImage_IsUniformColor_Sync(const std::string &imageFileName)
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


bool ReadScreenshot_IsImageEmpty(const std::string & sampleName)
{
  std::string filename = BABYLON::SamplesInfo::SampleScreenshotFile_Absolute(sampleName);
  return ReadImage_IsUniformColor_Sync(filename);
}


} // namespace Samples
} // namespace BABYLON
