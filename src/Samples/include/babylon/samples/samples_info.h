#ifndef BABYLONCPP_SAMPLES_INFO_H
#define BABYLONCPP_SAMPLES_INFO_H

#include <string>
#include <unordered_map>
#include <functional>
#include <memory>

namespace BABYLON {

class ICanvas;
class IRenderableScene;
using IRenderableScenePtr = std::shared_ptr<IRenderableScene>;

namespace SamplesInfo {

using CategoryName = std::string;
using SampleName = std::string;
using SampleFactoryFunction = std::function<IRenderableScenePtr(ICanvas* iCanvas)>;
using SamplesInCategory = std::unordered_map<SampleName, SampleFactoryFunction>;
using SamplesByCategory = std::unordered_map<CategoryName, SamplesInCategory>;


// These informations are extracted from to source file
struct SampleSourceInfo {
  std::string HeaderFile;
  std::string SourceFile;
  std::string Brief;
  std::vector<std::string> Links;
};

enum class SampleRunStatus {
  success,
  unhandledException,
  processHung,
  empty3d,
  broken3d,
  unknown
};

struct SampleRunInfo {
  SampleRunStatus sampleRunStatus = SampleRunStatus::unknown;
  std::string unhandledExceptionStackTrace = "";
  std::string broken3dComment;
  std::string screenshotFile;
};

struct SampleInfo {
  SampleSourceInfo sourceInfo;
  SampleRunInfo runInfo;
};

const SamplesByCategory & GetSamplesByCategory();
const std::vector<CategoryName> GetCategoryNames();
const std::vector<SampleName> GetSampleNames();

inline std::string screenshotsDirectory()
{
  return "ScreenShots/";
}


} // namespace SamplesInfo

} // namespace BABYLON

#endif // BABYLONCPP_SAMPLES_INFO_H
