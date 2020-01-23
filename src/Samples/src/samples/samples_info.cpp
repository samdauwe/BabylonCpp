#include <babylon/core/filesystem.h>
#include <babylon/samples/samples_auto_declarations.h>
#include <babylon/babylon_common.h>
#include <babylon/core/logging.h>
#include <magic_enum.hpp>
#include <cassert>

#include <babylon/samples/samples_info.h>

namespace BABYLON {


namespace SamplesInfo {

std::string screenshotsDirectory() {
  std::string directory = babylon_repo_folder() + "/assets/screenshots/ScreenshotsData";
  if (!Filesystem::isDirectory(directory))
    Filesystem::createDirectory(directory);
  return directory;
}

std::string SampleRunInfoFilePrefix(const SampleName& sampleName) {
  return screenshotsDirectory() + "/" + sampleName;
}

std::string SampleScreenshotFilename(const SampleName& sampleName) {
  return SampleRunInfoFilePrefix(sampleName) + ".jpg";
}

SamplesCollection& SamplesCollection::Instance() {
  static SamplesCollection instance;
  return instance;
}

SamplesCollection::SamplesCollection()
{
  auto registerSample = [this](const CategoryName & c, const SampleName & s, SampleFactoryFunction f) {
    SampleData sampleData;
    sampleData.factoryFunction = f;
    _allSamples[c][s] = sampleData;
  };

  BABYLON::Samples::auto_populate_samples(registerSample);
}

void SamplesCollection::SaveSampleRunInfo(
  const SampleName& sampleName,
  const SampleRunInfo& sampleRunInfo)
{
  SampleData *sampleData = GetSampleByName(sampleName);
  assert(sampleData != nullptr);
  sampleData->runInfo = sampleRunInfo;

  if (sampleRunInfo.sampleRunStatus != SampleRunStatus::success)
  {
    Filesystem::removeFile(SampleScreenshotFilename(sampleName));

    std::string_view statusName = magic_enum::enum_name(sampleRunInfo.sampleRunStatus);
    std::string content;
    if (sampleRunInfo.sampleRunStatus == SampleRunStatus::unhandledException)
      content = sampleRunInfo.unhandledExceptionStackTrace;
    if (sampleRunInfo.sampleRunStatus == SampleRunStatus::broken3d)
      content = sampleRunInfo.broken3dComment;
    std::string filename = SampleRunInfoFilePrefix(sampleName) + "." + std::string(statusName) + ".txt";
    Filesystem::writeFileContents(filename.c_str(), content);
  }
  //sampleRunInfo.sampleRunStatus
}

SampleData* SamplesCollection::GetSampleByName(const SampleName& sampleName)
{
  for (auto& [categoryName, samplesInCategory] : _allSamples)
  {
    for (auto& [currentSampleName, sampleData] : samplesInCategory)
    {
      if (currentSampleName == sampleName)
        return &sampleData;
    }
  }
  return nullptr;
}



} // namespace Samples

}
