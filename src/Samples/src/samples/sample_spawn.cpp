#include <babylon/core/timer.h>
#include <babylon/asio/internal/future_utils.h>

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


}
}