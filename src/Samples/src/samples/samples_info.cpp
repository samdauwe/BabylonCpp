#include <babylon/core/filesystem.h>
#include <babylon/samples/samples_auto_declarations.h>
#include <babylon/babylon_common.h>
#include <babylon/core/logging.h>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>
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

std::string SampleRunInfoStatusFile(const SampleName& sampleName, SampleRunStatus sampleRunStatus)
{
  std::string_view statusName = magic_enum::enum_name(sampleRunStatus);
  std::string filename = SampleRunInfoFilePrefix(sampleName) + "." + std::string(statusName) + ".txt";
  return filename;
}

SamplesCollection& SamplesCollection::Instance() {
  static SamplesCollection instance;
  return instance;
}

bool compareSampleData_CategoryThenName(const SampleData & s1, const SampleData & s2)
{
  if (s1.categoryName.compare(s2.categoryName) == 0)
    return (s1.sampleName.compare(s2.sampleName) < 0);
  else
    return s1.categoryName.compare(s2.categoryName) < 0;
}

SamplesCollection::SamplesCollection()
{
  auto registerSample = [this](const CategoryName & c, const SampleName & s, SampleFactoryFunction f) {
    SampleData sampleData;
    sampleData.categoryName = c;
    sampleData.sampleName = s;
    sampleData.factoryFunction = f;
    _allSamples.push_back(sampleData);
  };

  BABYLON::Samples::auto_populate_samples(registerSample);
  std::sort(_allSamples.begin(), _allSamples.end(), compareSampleData_CategoryThenName);

  for (auto & sampleData: _allSamples)
    sampleData.runInfo = SampleRunInfo_ReadFromDisk(sampleData.sampleName);

  BABYLON_LOG_INFO("SamplesCollection", "found", _allSamples.size(), " samples");
  BABYLON_LOG_INFO("SamplesCollection", "stats", GetSampleStatsString().c_str());
}


void SamplesCollection::SampleRunInfo_RemoveStatusFromDisk(const SampleName& sampleName)
{
  constexpr auto allRunStatuses = magic_enum::enum_values<SampleRunStatus>();
  for (const auto & runStatus: allRunStatuses)
  {
    std::string filename = SampleRunInfoStatusFile(sampleName, runStatus);
    Filesystem::removeFile(filename);
  }
}

SampleRunInfo SamplesCollection::SampleRunInfo_ReadFromDisk(const SampleName& sampleName)
{
  assert(GetSampleByName(sampleName) != nullptr);

  SampleRunInfo result;
  constexpr auto allRunStatuses = magic_enum::enum_values<SampleRunStatus>();
  for (const auto & runStatus: allRunStatuses)
  {
    std::string filename = SampleRunInfoStatusFile(sampleName, runStatus);
    if (Filesystem::isFile(filename)) {
      result.sampleRunStatus = runStatus;
      if (runStatus == SampleRunStatus::unhandledException) {
        result.unhandledExceptionStackTrace = Filesystem::readFileContents(filename.c_str());
      }
    }
  }

  if (result.sampleRunStatus == SampleRunStatus::unknown) {
    auto screenshotFile = SampleScreenshotFilename(sampleName);
    if (Filesystem::isFile(screenshotFile)) {
      result.screenshotFile = screenshotFile;
      result.sampleRunStatus = SampleRunStatus::success;
    }
  }
  return result;
}

void SamplesCollection::SampleRunInfo_Save(
  const SampleName& sampleName,
  const SampleRunInfo& sampleRunInfo)
{

  SampleData *sampleData = GetSampleByName(sampleName);
  assert(sampleData != nullptr);

  SampleRunInfo_RemoveStatusFromDisk(sampleName);
  sampleData->runInfo = sampleRunInfo;

  if (sampleRunInfo.sampleRunStatus != SampleRunStatus::success)
  {
    Filesystem::removeFile(SampleScreenshotFilename(sampleName));

    std::string_view statusName = magic_enum::enum_name(sampleRunInfo.sampleRunStatus);
    std::string content;
    if (sampleRunInfo.sampleRunStatus == SampleRunStatus::unhandledException)
      content = sampleRunInfo.unhandledExceptionStackTrace;
    std::string filename = SampleRunInfoFilePrefix(sampleName) + "." + std::string(statusName) + ".txt";
    Filesystem::writeFileContents(filename.c_str(), content);
  }
  //sampleRunInfo.sampleRunStatus
}

SampleData* SamplesCollection::GetSampleByName(const SampleName& sampleName)
{
  for (auto& sampleData : _allSamples)
      if (sampleData.sampleName == sampleName)
        return &sampleData;
  return nullptr;
}


SampleStats SamplesCollection::GetSampleStats()
{
  BABYLON::SamplesInfo::SampleStats stats;
  for (const auto& sampleData: _allSamples)
    ++stats[sampleData.runInfo.sampleRunStatus];
  return stats;
}

std::string SamplesCollection::GetSampleStatsString()
{
  auto stats = GetSampleStats();

  using namespace nlohmann;
  json j;
  constexpr auto allRunStatuses = magic_enum::enum_values<SampleRunStatus>();
  for (const auto & runStatus: allRunStatuses)
  {
    std::string_view status_name = magic_enum::enum_name(runStatus);
    j[std::string(status_name)] = stats[runStatus];
  }
  return j.dump();
}

} // namespace Samples

}
