#include <babylon/core/filesystem.h>
#include <babylon/samples/samples_auto_declarations.h>
#include <babylon/babylon_common.h>
#include <babylon/core/logging.h>
#include <babylon/asio/asio.h>
#include <babylon/misc/string_tools.h>
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
  std::optional<SampleAutoRunStatus> optionalStatus = magic_enum::enum_cast<EnumClass>(enumName);
  assert(optionalStatus.has_value());
  v = *optionalStatus;
}

void to_json(nlohmann::json& j, const SampleAutoRunStatus& v) {
  to_json_enum(j, v);
}
void from_json(const nlohmann::json& j, SampleAutoRunStatus& v) {
  from_json_enum(j, v);
}

void to_json(nlohmann::json& j, const SampleManualRunInfo& v)
{
  j["failing"] = v.failing;
  j["detail"] = v.detail;
}
void from_json(const nlohmann::json& j, SampleManualRunInfo& v)
{
  v.failing = j["failing"];
  v.detail = j["detail"];
}


std::string screenshotsDirectory() {
  std::string directory = assets_folder() + "/screenshots/ScreenshotsData";
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

std::string SamplesProjectFolder()
{
#ifndef __EMSCRIPTEN__
  std::string folder = babylon_repo_folder() + "/src/Samples/";
#else
  std::string folder = "/emscripten_static_assets_folder/Samples/";
#endif
  return folder;
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
  ReadAllSampleStatuses();

  // 4. Read source info
  ReadSamplesSourceInfos();

  // 5. Read Manual Run Status
  ReadSampleManualRunInfo();

  BABYLON_LOG_INFO("SamplesCollection", "found", _allSamples.size(), " samples");
  BABYLON_LOG_INFO("SamplesCollection", "stats", GetSampleStatsString().c_str());
}

void SamplesCollection::ReadSamplesSourceInfos()
{
  auto lowerCaseName = [](const std::string & sampleNameMixedCase) -> std::string
  {
    std::string sampleNameLowerCase;
    for (auto c : sampleNameMixedCase)
      sampleNameLowerCase += static_cast<char>(std::tolower(c));
    return sampleNameLowerCase;
  };

  auto onJsonErrorLoad = [](const std::string &errorMessage)
  {
    BABYLON_LOG_ERROR("SamplesInfoCache", "Error while reading samples_info.json ==>", errorMessage.c_str());
  };

  auto onJsonLoaded = [=](const std::string &jsonString)
  {
    auto json_all_samples_info = nlohmann::json::parse(jsonString);
    for (const auto& element : json_all_samples_info)
    {
      std::string sampleName = element["sample_name"];
      SampleSourceInfo sampleInfo;
      {
        sampleInfo.Brief      = element["brief"];
        sampleInfo.HeaderFile = element["header_file"];
        sampleInfo.SourceFile = element["source_file"];
        for (const auto& link : element["links"])
          sampleInfo.Links.push_back(link);
      }

      for (auto & sampleData: _allSamples)
      {
        if (lowerCaseName(sampleData.sampleName) == lowerCaseName(sampleName))
          sampleData.sourceInfo = sampleInfo;
      }
    }
  };

  BABYLON::asio::LoadUrlAsync_Text(assets_folder() + "/samples_info.json", onJsonLoaded, onJsonErrorLoad);

}


void SamplesCollection::ReadAllSampleStatuses()
{
  auto onStatusesLoaded = [this](const std::string& jsonString) {
    using namespace nlohmann;
    auto jsonData = json::parse(jsonString);
    auto allRunStatuses =  jsonData.get<std::map<SampleName, SampleAutoRunStatus>>();
    for (const auto &[sampleName, sampleRunStatus] : allRunStatuses)
    {
      SampleData * data = GetSampleByName(sampleName);
      //assert(data != nullptr);
      if (data)
        data->autoRunInfo.sampleRunStatus = sampleRunStatus;
    }
  };

  auto onError = [](const std::string &msg) {
    BABYLON_LOG_ERROR("SamplesCollection", "Could not read statuses: ", msg.c_str());
  };

  asio::LoadUrlAsync_Text(screenshotsDirectory() + "/aa_runStatus.json",
                          onStatusesLoaded, onError);

  asio::Service_WaitAll_Sync(); // Because we want to display stats right after the start
}


void SamplesCollection::SaveSampleManualRunInfo()
{
  using namespace nlohmann;
  std::map<SampleName, SampleManualRunInfo> allRunManualInfo;
  for (const auto& sampleData : _allSamples)
    allRunManualInfo[sampleData.sampleName] = sampleData.sampleManualRunInfo;

  std::ofstream ofs(screenshotsDirectory() + "/aa_sampleRunManualInfo.json");
  nlohmann::json j = allRunManualInfo;
  ofs << std::setw(4) << j << std::endl;
  ofs.close();
}

void SamplesCollection::ReadSampleManualRunInfo()
{
  auto onLoaded = [this](const std::string& jsonString) {
    using namespace nlohmann;
    auto jsonData = json::parse(jsonString);

    auto allRunManualInfo =  jsonData.get<std::map<SampleName, SampleManualRunInfo>>();
    for (const auto &[sampleName, sampleRunManualInfo] : allRunManualInfo)
    {
      SampleData * data = GetSampleByName(sampleName);
      //assert(data != nullptr);
      if (data)
        data->sampleManualRunInfo = sampleRunManualInfo;
    }
  };

  auto onError = [](const std::string &msg) {
    BABYLON_LOG_ERROR("SamplesCollection", "Could not read sampleRunManualInfo: ", msg.c_str());
  };

  asio::LoadUrlAsync_Text(screenshotsDirectory() + "/aa_sampleRunManualInfo.json",
                          onLoaded, onError);
}

void SamplesCollection::SetSampleManualRunInfo(
  const SampleName& sampleName, const SampleManualRunInfo& sampleManualRunInfo)
{
  SampleData *sampleData = GetSampleByName(sampleName);
  assert(sampleData != nullptr);
  sampleData->sampleManualRunInfo = sampleManualRunInfo;
  SaveSampleManualRunInfo();
}

void SamplesCollection::SetSampleRunInfo(
  const SampleName& sampleName,
  const SampleAutoRunInfo& sampleRunInfo)
{
  SampleData *sampleData = GetSampleByName(sampleName);
  assert(sampleData != nullptr);
  sampleData->autoRunInfo = sampleRunInfo;
  if (sampleRunInfo.sampleRunStatus != SampleAutoRunStatus::success) {
    Filesystem::removeFile(SampleScreenshotFilename(sampleName));
  }
}

void SamplesCollection::SaveAllSamplesRunStatuses()
{
  std::map<SampleName, SampleAutoRunStatus> allRunStatuses;
  for (const auto & sampleData : _allSamples)
    allRunStatuses[sampleData.sampleName] = sampleData.autoRunInfo.sampleRunStatus;

  std::ofstream ofs(screenshotsDirectory() + "/aa_runStatus.json");
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

const SampleData* SamplesCollection::GetSampleByName(const SampleName& sampleName) const
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
    ++stats[sampleData.autoRunInfo.sampleRunStatus];
  return stats;
}

std::string SamplesCollection::GetSampleStatsString()
{
  auto stats = GetSampleStats();

  using namespace nlohmann;
  json j;
  constexpr auto allRunStatuses = magic_enum::enum_values<SampleAutoRunStatus>();
  for (const auto & runStatus: allRunStatuses)
  {
    std::string_view status_name = magic_enum::enum_name(runStatus);
    j[std::string(status_name)] = stats[runStatus];
  }
  return j.dump();
}

IRenderableScenePtr SamplesCollection::createRenderableScene(
  const std::string& sampleName, ICanvas* iCanvas) const
{
  const SampleData * sampleData = GetSampleByName(sampleName);
  assert(sampleData != nullptr);
  auto r = sampleData->factoryFunction(iCanvas);
  return r;
}

std::map<SamplesInfo::CategoryName, std::vector<const SampleData *>>
  SamplesCollection::SearchSamples(const SampleSearchQuery& query)
{
  std::map<SamplesInfo::CategoryName, std::vector<const SampleData *>> r;

  auto matchString = [](const std::string& whatToSearch, const std::string& whereToSearch) -> bool
  {
    bool match = true;
    auto searchItems = StringTools::split(whatToSearch, ' ');
    for (const std::string &searchItem: searchItems) {
      std::string s1 = StringTools::toLowerCase(searchItem);
      std::string s2 = StringTools::toLowerCase(whereToSearch);
      if (s2.find(s1) == std::string::npos)
        match = false;
    }
    return match;
  };

  for (const auto& sampleData : _allSamples)
  {
    bool match = true;
    if ( !matchString(query.Query,
      sampleData.sampleName + " " + sampleData.categoryName + " " + sampleData.sampleManualRunInfo.detail))
      match = false;

    if (!query.OnlyManualRunFailure)
    {
      if (sampleData.sampleManualRunInfo.failing)
        match = false;
      for (const auto & [sampleRunStatus, flagInclude] : query.IncludeStatus)
      {
        if ((sampleData.autoRunInfo.sampleRunStatus == sampleRunStatus)  && (!flagInclude))
          match = false;
      }
    }

    if (query.OnlyManualRunFailure && !sampleData.sampleManualRunInfo.failing)
      match = false;

    if (match)
      r[sampleData.categoryName].push_back(&sampleData);
  }
  return r;
}

} // namespace Samples

}
