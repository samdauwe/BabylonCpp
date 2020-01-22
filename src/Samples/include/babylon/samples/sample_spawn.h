#ifndef BABYLONCPP_SAMPLE_SPAWN_H
#define BABYLONCPP_SAMPLE_SPAWN_H

#include <string>
#include <vector>


namespace BABYLON {
namespace Samples {

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
);

}
}

#endif // BABYLONCPP_SAMPLE_SPAWN_H
