#include <babylon/samples/samples_index.h>

#include <babylon/asio/asio.h>
#include <babylon/babylon_common.h>
#include <babylon/samples/babylon_register_sample.h>
#include <babylon/babylon_stl_util.h>
#include <babylon/core/string.h>
#include <babylon/core/logging.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/babylon_common.h>
#include <babylon/samples/samples_auto_declarations.h>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

namespace BABYLON {
namespace Samples {

SamplesIndex::SamplesIndex() = default;

void SamplesIndex::fillSamplesFailures() const
{
  static bool done = false;
  if (done)
    return;

  _samplesFailures = {
    {"BlurModeForMirrorsScene", {SampleFailureReasonKind::empty3d}},
    {"EnvironmentTextureScene", {SampleFailureReasonKind::empty3d}},
    {"FurMaterialScene", {SampleFailureReasonKind::broken3d, "Black fur!"}},
    {"InnerMeshPointsScene", {SampleFailureReasonKind::broken3d, "Low FPS! Less than 1"}},
    {"IsPointInsideMeshScene", {SampleFailureReasonKind::processHung, "infinite loop inside Mesh::pointIsInside"}},
    {"MultiSampleRenderTargetsScene", {SampleFailureReasonKind::empty3d}},
    {"SimpleSkinScene", {SampleFailureReasonKind::empty3d}},
    {"VolumetricLightScatteringScene", {SampleFailureReasonKind::broken3d, "No scatter / 3d emtpy after dragging"}}
  };

  done = true;
}

SamplesIndex& SamplesIndex::Instance()
{
  static SamplesIndex instance;
  static bool wasPopulated = false;
  if (!wasPopulated) {
    wasPopulated = true;
    auto_populate_samples(instance);
  }
  return instance;
}

SamplesIndex::~SamplesIndex() = default;

void SamplesIndex::RegisterSample(const std::string& categoryName, const std::string& sampleName,
                                  SampleFactoryFunction fn)
{
  _samplesIndex[categoryName].addSample(sampleName, fn);
}

std::optional<BABYLON::Samples::SampleFailureReason>
SamplesIndex::doesSampleFail(const std::string& sampleName) const
{
  fillSamplesFailures();

  if (_samplesFailures.find(sampleName) == _samplesFailures.end())
    return std::nullopt;
  else
    return _samplesFailures.at(sampleName);
}

bool SamplesIndex::sampleExists(const std::string& sampleName) const
{
  for (const auto& item : _samplesIndex) {
    if (stl_util::contains(item.second.samples(), sampleName)) {
      return true;
    }
  }

  return false;
}

class SamplesInfoCache
{
public:
  std::shared_ptr<SampleInfo> getSampleInfo(const std::string& sampleNameMixedCase)
  {
    std::string name = lowerCaseName(sampleNameMixedCase);
    if (cacheSampleInfo_.find(name) == cacheSampleInfo_.end())
      cacheSampleInfo_[name] = std::make_shared<SampleInfo>();
    return cacheSampleInfo_.at(name);
  }

  static SamplesInfoCache& instance()
  {
    static SamplesInfoCache instance;
    return instance;
  }

private:
  SamplesInfoCache()
  {
    std::cout << "SamplesInfoCache() 1 \n";
    auto onSuccessLoad = [this](const std::string& jsonString) {
      this->onJsonLoaded(jsonString);
    };
    auto onErrorLoad = [this](const std::string& errorMessage) {
      this->onJsonErrorLoad(errorMessage);
    };
    std::cout << "SamplesInfoCache() 2 \n";
    BABYLON::asio::LoadUrlAsync_Text("samples_info.json", onSuccessLoad, onErrorLoad);
    std::cout << "SamplesInfoCache() 3 \n";
  }

  std::string lowerCaseName(const std::string & sampleNameMixedCase)
  {
    std::string sampleNameLowerCase;
    for (auto c : sampleNameMixedCase)
      sampleNameLowerCase += static_cast<char>(std::tolower(c));
    return sampleNameLowerCase;
  }

  void onJsonErrorLoad(const std::string &errorMessage)
  {
    BABYLON_LOG_ERROR("SamplesInfoCache", "Error while reading samples_info.json ==>", errorMessage.c_str());
  }

  void onJsonLoaded(const std::string &jsonString)
  {
    auto json_all_samples_info = nlohmann::json::parse(jsonString);
    for (const auto& element : json_all_samples_info)
    {
      std::string name = element["sample_name"];
      std::shared_ptr<SampleInfo> sampleInfo = getSampleInfo(name);
      sampleInfo->Brief      = element["brief"];
      sampleInfo->HeaderFile = element["header_file"];
      sampleInfo->SourceFile = element["source_file"];
      for (const auto& link : element["links"]) {
        sampleInfo->Links.push_back(link);
      }
    }
  }

  std::map<std::string, std::shared_ptr<SampleInfo>> cacheSampleInfo_;
  bool wasJsonLoadStarted = false;
};

std::shared_ptr<SampleInfo> SamplesIndex::getSampleInfo(const std::string& sampleName) const
{
  return SamplesInfoCache::instance().getSampleInfo(sampleName);
}

std::vector<std::string> SamplesIndex::getSampleNames() const
{
  // Extract the enabled sample names from the map
  std::vector<std::string> sampleNames;
  for (const auto& samplesCategory : _samplesIndex) {
    for (const auto& element : samplesCategory.second.samples()) {
      sampleNames.emplace_back(element.first);
    }
  }

  // Sort the vector with sample names in ascending order
  std::sort(sampleNames.begin(), sampleNames.end());

  return sampleNames;
}

std::vector<std::string> SamplesIndex::getCategoryNames() const
{
  // Extract the category names
  auto categoryNames = stl_util::extract_keys(_samplesIndex);

  // Sort the vector with category names in ascending order
  std::sort(categoryNames.begin(), categoryNames.end());

  return categoryNames;
}

bool SamplesIndex::categoryExists(const std::string& categoryNameToSearch) const
{
  bool _categoryExists = false;

  // Extract the category names
  auto categoryNames = stl_util::extract_keys(_samplesIndex);

  // Search for sample
  for (const auto& categoryName : categoryNames) {
    if (categoryName == categoryNameToSearch) {
      _categoryExists = true;
      break;
    }
  }

  return _categoryExists;
}

std::vector<std::string>
SamplesIndex::getSampleNamesInCategory(const std::string& categoryName) const
{
  // Extract the enabled sample names for the given category from the map
  std::vector<std::string> sampleNames;
  if (stl_util::contains(_samplesIndex, categoryName)) {
    const auto& samplesCategory = _samplesIndex.at(categoryName);
    for (const auto& element : samplesCategory.samples()) {
      sampleNames.emplace_back(element.first);
    }
  }

  // Sort the vector with sample names in ascending order
  std::sort(sampleNames.begin(), sampleNames.end());

  return sampleNames;
}

IRenderableScenePtr SamplesIndex::createRenderableScene(const std::string& sampleName,
                                                        ICanvas* iCanvas) const
{
  for (const auto& item : _samplesIndex) {
    if (stl_util::contains(item.second.samples(), sampleName)) {
      return item.second.samples().at(sampleName)(iCanvas);
    }
  }

  return nullptr;
}

void SamplesIndex::listSamples()
{
  auto categories = getCategoryNames();
  for (const auto& category : categories) {
    std::cout << "********************************************"
              << "\n";
    std::cout << "Category: " << category << "\n";
    std::cout << "********************************************"
              << "\n";
    auto samples = getSampleNamesInCategory(category);
    for (const auto& sample : samples) {
      std::cout << sample << "\n";
    }
  }
}

std::string SampleFailureReason_Str(SampleFailureReasonKind s)
{
  switch (s) {
    case SampleFailureReasonKind::segFault:
      return "Segmentation fault";
    case SampleFailureReasonKind::processHung:
      return "Process hung (infinite loop?)";
    case SampleFailureReasonKind::empty3d:
      return "3D rendering is empty";
    case SampleFailureReasonKind::broken3d:
      return "Broken (bad rendering and/or bad behavior)";
    default:
      throw "Unhandled enum!";
  }
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


} // end of namespace Samples
} // end of namespace BABYLON
