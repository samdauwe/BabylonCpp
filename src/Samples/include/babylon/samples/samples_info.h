#ifndef BABYLONCPP_SAMPLES_INFO_H
#define BABYLONCPP_SAMPLES_INFO_H

#include <string>
#include <unordered_map>
#include <map>
#include <functional>
#include <memory>
#include <optional>
#include <babylon/babylon_api.h>

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

enum class BABYLON_SHARED_EXPORT SampleRunStatus {
  success,
  unhandledException,
  tooSlowOrHung,
  empty3d,
  unknown
};

struct BABYLON_SHARED_EXPORT SampleSearchQuery {
  std::string QueryString = "";
  std::string QueryCategory = "";
  std::map<SampleRunStatus, bool> IncludeStatus = {
    { SampleRunStatus::success, true },
    { SampleRunStatus::unhandledException, true },
    { SampleRunStatus::tooSlowOrHung, true },
    { SampleRunStatus::empty3d, true },
    { SampleRunStatus::unknown, true },
  };
};

struct BABYLON_SHARED_EXPORT SampleRunInfo {
  SampleRunStatus sampleRunStatus = SampleRunStatus::unknown;
  std::string unhandledExceptionStackTrace = "";
  std::string screenshotFile;
};

struct BABYLON_SHARED_EXPORT SampleData {
  CategoryName categoryName;
  SampleName sampleName;
  SampleSourceInfo sourceInfo;
  SampleRunInfo runInfo;
  SampleFactoryFunction factoryFunction = {};
};

using SampleStats = std::map<SampleRunStatus, std::size_t>;

class BABYLON_SHARED_EXPORT SamplesCollection
{
public:
  static SamplesCollection& Instance();

  const std::vector<SampleData>& AllSamples() const { return _allSamples; }

  void SaveSampleRunInfo(const SampleName& sampleName, const SampleRunInfo& sampleRunInfo);

  SampleData* GetSampleByName(const SampleName& sampleName); // returns nullptr if not found

  //  SamplesByCategory SearchSamplesByCategory(const SampleSearchQuery & query = {});
//

private:
  SamplesCollection();
  std::vector<SampleData> _allSamples;
};


BABYLON_SHARED_EXPORT std::string screenshotsDirectory();

} // namespace SamplesInfo

} // namespace BABYLON

#endif // BABYLONCPP_SAMPLES_INFO_H
