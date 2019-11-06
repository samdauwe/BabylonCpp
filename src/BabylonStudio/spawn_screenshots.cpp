#include <babylon/samples/samples_index.h>
#include <babylon/core/logging.h>
#include <iostream>

#ifdef _WIN32
#include <process.h>
#endif

namespace BABYLON {
namespace impl {

#ifdef _WIN32
void spawnScreenshots(const std::string & exeName, bool skipFailing)
{
  BABYLON::Samples::SamplesIndex& samplesIndex = Samples::SamplesIndex::Instance();
  auto samples = samplesIndex.getSampleNames();
  for (auto sample : samples)
  {
    BABYLON_LOG_INFO("ScreenshotAllSamples", sample);
    auto does_fail = samplesIndex.doesSampleFail(sample);
    if (does_fail && skipFailing) {
      BABYLON_LOG_INFO("ScreenshotAllSamples", "Skipping failing ", sample, ": ", SampleFailureReason_Str(does_fail.value().Kind));
    }
    else
      _spawnl(P_WAIT, exeName.c_str(), exeName.c_str(), "-s", sample.c_str(), "-p", NULL);
  }
}
#else

#pragma message("spawnScreenshots is not yet implmented on *nixes")

void spawnScreenshots(const std::string & , bool)
{
  std::cerr << "spawnScreenshots is not yet implmented on *nixes";
}
#endif

} // namespace impl
} // namespace BABYLON
