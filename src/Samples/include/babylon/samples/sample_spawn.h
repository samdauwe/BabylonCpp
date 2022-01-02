#ifndef BABYLONCPP_SAMPLE_SPAWN_H
#define BABYLONCPP_SAMPLE_SPAWN_H

#include <babylon/babylon_api.h>
#include <string>
#include <vector>


namespace BABYLON {
namespace Samples {

struct BABYLON_SHARED_EXPORT SpawnOptions {
  bool        InheritEnvironment = true;
  bool        MixStdOutStdErr = true;
  double      MaxExecutionTimeSeconds = -1;
  bool        CopyOutputToMainProgramOutput  = false;
};

struct BABYLON_SHARED_EXPORT SpawnResult
{
  int ExitStatus = -99;
  bool MaxExecutionTimePassed = false;
  std::string StdOutErr = "";
};

BABYLON_SHARED_EXPORT SpawnResult SpawnWaitSubProcess(
  const std::vector<std::string> & command,
  const SpawnOptions& spawnOptions
);


BABYLON_SHARED_EXPORT bool ReadScreenshot_IsImageEmpty(const std::string & sampleName);

} // namespace Samples
} // namespace BABYLON

#endif // BABYLONCPP_SAMPLE_SPAWN_H
