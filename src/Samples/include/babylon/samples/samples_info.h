#ifndef BABYLONCPP_SAMPLES_INFO_H
#define BABYLONCPP_SAMPLES_INFO_H

#include <string>
#include <unordered_map>
#include <map>
#include <functional>
#include <memory>
#include <optional>
#include <babylon/babylon_api.h>
#include <nlohmann/json.hpp>

namespace BABYLON {

class ICanvas;
class IRenderableScene;
using IRenderableScenePtr = std::shared_ptr<IRenderableScene>;

namespace SamplesInfo {

using CategoryName = std::string;
using SampleName = std::string;
using SampleFactoryFunction = std::function<IRenderableScenePtr(ICanvas* iCanvas)>;


// These informations are extracted from to source file
struct BABYLON_SHARED_EXPORT SampleSourceInfo {
  std::string HeaderFile;
  std::string SourceFile;
  std::string Brief;
  std::vector<std::string> Links;
};

enum class BABYLON_SHARED_EXPORT SampleAutoRunStatus {
  success,
  unhandledException,
  tooSlowOrHung,
  empty3d,
  unknown
};
void BABYLON_SHARED_EXPORT to_json(nlohmann::json& j, const SampleAutoRunStatus& v);
void BABYLON_SHARED_EXPORT from_json(const nlohmann::json& j, SampleAutoRunStatus& v);

struct BABYLON_SHARED_EXPORT SampleSearchQuery {
  std::string Query = "";
  bool OnlyManualRunFailure                         = false;
  std::map<SampleAutoRunStatus, bool> IncludeStatus = {
    {SampleAutoRunStatus::success, true },
    {SampleAutoRunStatus::unhandledException, false },
    {SampleAutoRunStatus::tooSlowOrHung, false },
    {SampleAutoRunStatus::empty3d, false },
    {SampleAutoRunStatus::unknown, false },
  };
};

struct BABYLON_SHARED_EXPORT SampleAutoRunInfo {
  SampleAutoRunStatus sampleRunStatus = SampleAutoRunStatus::unknown;
  std::string unhandledExceptionStackTrace = "";
};

struct BABYLON_SHARED_EXPORT SampleManualRunInfo {
  bool failing = false;
  std::string detail = "";
};
void BABYLON_SHARED_EXPORT to_json(nlohmann::json& j, const SampleManualRunInfo& v);
void BABYLON_SHARED_EXPORT from_json(const nlohmann::json& j, SampleManualRunInfo& v);


struct BABYLON_SHARED_EXPORT SampleData {
  CategoryName categoryName;
  SampleName sampleName;
  SampleSourceInfo sourceInfo;
  SampleAutoRunInfo autoRunInfo;
  SampleManualRunInfo sampleManualRunInfo;
  SampleFactoryFunction factoryFunction = {};
};

using SampleStats = std::map<SampleAutoRunStatus, std::size_t>;


class BABYLON_SHARED_EXPORT SamplesCollection
{
public:
  static SamplesCollection& Instance();

  //
  // Browsing API
  //
  const std::vector<SampleData>& AllSamples() const { return _allSamples; }
  std::map<SamplesInfo::CategoryName, std::vector<const SampleData *>> SearchSamples(
    const SampleSearchQuery& query);

  SampleData* GetSampleByName(const SampleName& sampleName); // returns nullptr if not found
  const SampleData* GetSampleByName(const SampleName& sampleName) const; // returns nullptr if not found

  IRenderableScenePtr createRenderableScene(const std::string& sampleName, ICanvas* iCanvas) const;

  //
  // SpawnScreenshot API
  //
  void SetSampleRunInfo(const SampleName& sampleName, const SampleAutoRunInfo& sampleRunInfo);
  void SetSampleManualRunInfo(const SampleName& sampleName, const SampleManualRunInfo& sampleManualRunInfo);
  void SaveAllSamplesRunStatuses();

private:
  SamplesCollection();

  SampleStats GetSampleStats();
  std::string GetSampleStatsString();

  void ReadSamplesSourceInfos();
  void ReadAllSampleStatuses();
  void ReadSampleManualRunInfo();
  void SaveSampleManualRunInfo();

  std::vector<SampleData> _allSamples;
};


BABYLON_SHARED_EXPORT std::string screenshotsDirectory();
BABYLON_SHARED_EXPORT std::string SamplesProjectFolder();

} // namespace SamplesInfo

} // namespace BABYLON

#endif // BABYLONCPP_SAMPLES_INFO_H
