#include <babylon/samples/samples_auto_declarations.h>

#include <babylon/samples/samples_info.h>

namespace BABYLON {


namespace SamplesInfo {

std::map<SampleRunStatus, std::string> SampleRunStatusNames = {
  {SampleRunStatus::success, "success"},
  {SampleRunStatus::unhandledException, "unhandledException"},
  {SampleRunStatus::tooSlowOrHung, "tooSlowOrHung"},
  {SampleRunStatus::empty3d, "empty3d"},
  {SampleRunStatus::broken3d, "broken3d"},
  {SampleRunStatus::unknown, "unknown"},
};

std::string SampleRunStatusName(SampleRunStatus &s)
{
  return SampleRunStatusNames.at(s);
}

const SamplesByCategory & AllSamples()
{
  static SamplesByCategory instance;
  static bool wasInitialized = false;
  if (!wasInitialized) {

    auto registerSample = [](const CategoryName & c, const SampleName & s, SampleFactoryFunction f) {
      SampleData sampleData;
      sampleData.factoryFunction = f;
      instance[c][s] = sampleData;
    };

    BABYLON::Samples::auto_populate_samples(registerSample);
    wasInitialized = true;
  }
  return instance;
}


std::string screenshotsDirectory()
{
  return "ScreenShots/";
}


} // namespace Samples

}
