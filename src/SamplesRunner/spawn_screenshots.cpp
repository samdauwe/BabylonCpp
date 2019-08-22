#include <babylon/samples/samples_index.h>
#include <babylon/core/logging.h>

#ifdef _WIN32
#include <process.h>
#endif

namespace BABYLON {
namespace impl {

void spawnScreenshots(const std::string & exeName, bool skipFailing)
{
  BABYLON::Samples::SamplesIndex samplesIndex;
  auto samples = samplesIndex.getSampleNames();
  for (auto sample : samples)
  {
    BABYLON_LOG_INFO("ScreenshotAllSamples", sample);
    auto does_fail = samplesIndex.doesSampleFail(sample);
    if (does_fail && skipFailing) {
      BABYLON_LOG_INFO("ScreenshotAllSamples", "Skipping failing ", sample, ": ", SampleFailureReason_Str(does_fail.value()));
    }
    else
      _spawnl(P_WAIT, exeName.c_str(), exeName.c_str(), "-s", sample.c_str(), "-p", NULL);
  }
}

} // namespace impl
} // namespace BABYLON
