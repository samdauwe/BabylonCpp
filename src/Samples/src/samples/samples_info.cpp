#include <babylon/core/filesystem.h>
#include <babylon/samples/samples_auto_declarations.h>
#include <babylon/babylon_common.h>
#include <babylon/core/logging.h>
#include <magic_enum.hpp>
#include <nlohmann/json.hpp>
#include <iostream>
#include <iomanip>
#include <cassert>


#include <babylon/samples/samples_info.h>

namespace BABYLON {


namespace SamplesInfo {

template<typename EnumClass>
void to_json_enum(nlohmann::json& j, const EnumClass& v) {
  std::string_view sv = magic_enum::enum_name(v);
  //std::string statusName(sv);
  j = sv;
}

template<typename EnumClass>
void from_json_enum(const nlohmann::json& j, EnumClass& v) {
  std::string enumName = j.get<std::string>();
  std::optional<SampleRunStatus> optionalStatus = magic_enum::enum_cast<EnumClass>(enumName);
  assert(optionalStatus.has_value());
  v = *optionalStatus;
}

void to_json(nlohmann::json& j, const SampleRunStatus& v) {
  to_json_enum(j, v);
}
void from_json(const nlohmann::json& j, SampleRunStatus& v) {
  from_json_enum(j, v);
}


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
  // 1. Call auto_populate
  {
    auto registerSample = [this](const CategoryName & c, const SampleName & s, SampleFactoryFunction f) {
      SampleData sampleData;
      sampleData.categoryName = c;
      sampleData.sampleName = s;
      sampleData.factoryFunction = f;
      _allSamples.push_back(sampleData);
    };

    BABYLON::Samples::auto_populate_samples(registerSample);
  }

  // 2. Sort by catgory, then name
  std::sort(_allSamples.begin(), _allSamples.end(), compareSampleData_CategoryThenName);

  // 3. Read Run status
//  for (auto & sampleData: _allSamples)
//    sampleData.runInfo = SampleRunInfo_ReadFromDisk(sampleData.sampleName);

  // 4. Read source info
  {

  }

  BABYLON_LOG_INFO("SamplesCollection", "found", _allSamples.size(), " samples");
  BABYLON_LOG_INFO("SamplesCollection", "stats", GetSampleStatsString().c_str());
}


void SamplesCollection::SetSampleRunInfo(
  const SampleName& sampleName,
  const SampleRunInfo& sampleRunInfo)
{
  SampleData *sampleData = GetSampleByName(sampleName);
  assert(sampleData != nullptr);
  sampleData->runInfo = sampleRunInfo;
  if (sampleRunInfo.sampleRunStatus != SampleRunStatus::success) {
    Filesystem::removeFile(SampleScreenshotFilename(sampleName));
  }
}

void SamplesCollection::SaveAllSamplesRunInfo()
{
  std::map<SampleName, SampleRunStatus> allRunStatuses;
  for (const auto & sampleData : _allSamples)
    allRunStatuses[sampleData.sampleName] = sampleData.runInfo.sampleRunStatus;

  std::ofstream ofs(screenshotsDirectory() + "/aa_runInfo.json");
  nlohmann::json j = allRunStatuses;
  ofs << std::setw(4) << j << std::endl;
  ofs.close();
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
