#include <babylon/samples/samples_index.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/string.h>
#include <babylon/interfaces/irenderable_scene.h>

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
    {"CircleCurvesFromBeziersScene", {SampleFailureReasonKind::empty3d, "Rendering empty with 50% chance"}},
    {"EnvironmentTextureScene", {SampleFailureReasonKind::empty3d}},
    {"FurMaterialScene", {SampleFailureReasonKind::segFault}},
    {"InnerMeshPointsScene", {SampleFailureReasonKind::segFault}},
    {"IsPointInsideMeshScene", {SampleFailureReasonKind::processHung}},
    {"LevelOfDetailScene", {SampleFailureReasonKind::segFault, "Low FPS / Segfault when dragging the mouse"}},
    {"LinesMeshSpiralScene", {SampleFailureReasonKind::empty3d}},
    {"LorenzAttractorScene", {SampleFailureReasonKind::empty3d, "Rendering empty with 50% chance"}},
    {"MergedMeshesScene", {SampleFailureReasonKind::segFault}},
    {"MotionBlurPostProcessScene", {SampleFailureReasonKind::empty3d}},
    {"MultiSampleRenderTargetsScene", {SampleFailureReasonKind::empty3d}},
    {"PointsCloudScene", {SampleFailureReasonKind::empty3d}},
    {"ProceduralHexPlanetGenerationScene", {SampleFailureReasonKind::broken3d, "Very small planet..."}},
    {"SimpleSkinScene", {SampleFailureReasonKind::empty3d}},
    {"ShaderMaterialWarpSpeedScene", {SampleFailureReasonKind::empty3d}},
    {"VolumetricLightScatteringScene", {SampleFailureReasonKind::segFault}}
  };

  done = true;
}

SamplesIndex& SamplesIndex::Instance()
{
  static SamplesIndex instance;
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

nlohmann::json ReadSampleInfoFile()
{
  std::ifstream is("samples_info.json");
  if (!is.good())
    is = std::ifstream("../samples_info.json");
  if (!is.good())
    is = std::ifstream("../../samples_info.json");
  if (is.good()) {
    nlohmann::json j;
    is >> j;
    return j;
  }
  else
    return nlohmann::json();
}

SampleInfo SamplesIndex::getSampleInfo(const std::string& sampleNameMixedCase) const
{
  static nlohmann::json _samplesInfo = ReadSampleInfoFile();
  static std::map<std::string, SampleInfo> cache;

  std::string sampleNameLowerCase;
  for (auto c : sampleNameMixedCase)
    sampleNameLowerCase += static_cast<char>(std::tolower(c));

  if (cache.find(sampleNameLowerCase) != cache.end())
    return cache.at(sampleNameLowerCase);

  SampleInfo result;
  for (const auto& element : _samplesInfo) {
    if (element["sample_name"] == sampleNameLowerCase) {
      result.Brief      = element["brief"];
      result.HeaderFile = element["header_file"];
      result.SourceFile = element["source_file"];
      for (const auto& link : element["links"]) {
        result.Links.push_back(link);
      }
    }
  }
  cache[sampleNameLowerCase] = result;
  return result;
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

} // end of namespace Samples
} // end of namespace BABYLON
