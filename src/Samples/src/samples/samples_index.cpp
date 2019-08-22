#include <babylon/samples/samples_index.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/core/string.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/samples/animations/_animations_samples_index.h>
#include <babylon/samples/cameras/_cameras_samples_index.h>
#include <babylon/samples/collisionsandintersections/_collisions_and_intersections_samples_index.h>
#include <babylon/samples/extensions/_extensions_samples_index.h>
#include <babylon/samples/lights/_lights_samples_index.h>
#include <babylon/samples/loaders/_loaders_samples_index.h>
#include <babylon/samples/materials/_materials_samples_index.h>
#include <babylon/samples/materialslibrary/_materials_library_samples_index.h>
#include <babylon/samples/meshes/_meshes_samples_index.h>
#include <babylon/samples/optimizations/_optimizations_samples_index.h>
#include <babylon/samples/particles/_particles_samples_index.h>
#include <babylon/samples/proceduraltextureslibrary/_procedural_textures_library_samples_index.h>
#include <babylon/samples/shadows/_shadows_samples_index.h>
#include <babylon/samples/specialfx/_special_fx_samples_index.h>
#include <babylon/samples/textures/_textures_samples_index.h>

#include <nlohmann/json.hpp>
#include <iostream>
#include <fstream>

namespace BABYLON {
namespace Samples {

SamplesIndex::SamplesIndex()
{
  _samplesFailures = {
  {"BasicElementsScene", SampleFailureReason::outOfBoundAccess},
  {"BlurModeForMirrorsScene", SampleFailureReason::outOfBoundAccess},
  {"BuildingInstancesGridScene", SampleFailureReason::processHung},
  {"BulbSelfShadowScene", SampleFailureReason::outOfBoundAccess},
  {"CannonBallScene", SampleFailureReason::outOfBoundAccess},
  {"CircleCurvesFromBeziersScene", SampleFailureReason::invalidComparator},
  {"ColoredRibbonScene", SampleFailureReason::outOfBoundAccess},
  {"ConvolutionPostProcessScene", SampleFailureReason::outOfBoundAccess},
  {"CurvedHelixMeshesScene", SampleFailureReason::outOfBoundAccess},
  {"EquirectangularMapAsReflectionTextureScene", SampleFailureReason::outOfBoundAccess},
  {"ExtrusionScene", SampleFailureReason::outOfBoundAccess},
  {"FireworksWithShaderCodeScene", SampleFailureReason::outOfBoundAccess},
  {"FogScene", SampleFailureReason::outOfBoundAccess},
  {"FresnelScene", SampleFailureReason::invalidComparator},
  {"GlassWubbleBallScene", SampleFailureReason::vectorIteratorInvalid},
  {"KernelBasedBlurScene", SampleFailureReason::outOfBoundAccess},
  {"LatheScene", SampleFailureReason::outOfBoundAccess},
  {"LightsScene", SampleFailureReason::outOfBoundAccess},
  {"MandelbrotFractalScene", SampleFailureReason::outOfBoundAccess},
  {"MirrorsScene", SampleFailureReason::vectorIteratorInvalid},
  {"MultiSampleRenderTargetsScene", SampleFailureReason::vectorIteratorInvalid},
  {"MultiViewsScene", SampleFailureReason::invalidComparator},
  {"PBRReflectionScene", SampleFailureReason::outOfBoundAccess},
  {"RayHelperScene", SampleFailureReason::invalidComparator},
  {"RollercoasterScene", SampleFailureReason::outOfBoundAccess},
  {"ShaderMaterialCloudsScene", SampleFailureReason::invalidComparator},
  {"SimplePostProcessRenderPipelineScene", SampleFailureReason::outOfBoundAccess},
  {"SphericalReflectionTextureScene", SampleFailureReason::vectorIteratorInvalid},
  {"TubeScene", SampleFailureReason::outOfBoundAccess},
  {"XRayMaterialWithFresnelSkullScene", SampleFailureReason::invalidComparator},
  };

  // Initialize the samples index
  _samplesIndex = {
    // Animations samples
    {_AnimationsSamplesIndex::CategoryName(), _AnimationsSamplesIndex()},
    // Cameras samples
    {_CamerasSamplesIndex::CategoryName(), _CamerasSamplesIndex()},
    // Collisions and Intersections samples
    {_CollisionsAndIntersectionsSamplesIndex::CategoryName(),
     _CollisionsAndIntersectionsSamplesIndex()},
    // Extensions samples
    {_ExtensionsSamplesIndex::CategoryName(), _ExtensionsSamplesIndex()},
    // Lights samples
    {_LightsSamplesIndex::CategoryName(), _LightsSamplesIndex()},
    // Loaders - babylon format
    {_LoadersBabylonSamplesIndex::CategoryName(),
     _LoadersBabylonSamplesIndex()},
#ifdef WITH_LOADERS
    // Loaders - glTF format
    {_LoadersGLTFSamplesIndex::CategoryName(), _LoadersGLTFSamplesIndex()},
#endif
    // Materials samples
    {_MaterialsSamplesIndex::CategoryName(), _MaterialsSamplesIndex()},
    // // Materials Library samples
    {_MaterialsLibrarySamplesIndex::CategoryName(),
     _MaterialsLibrarySamplesIndex()},
    // Meshes samples
    {_MeshesSamplesIndex::CategoryName(), _MeshesSamplesIndex()},
    // Optimizations samples
    {_OptimizationsSamplesIndex::CategoryName(), _OptimizationsSamplesIndex()},
    // Particles samples
    {_ParticlesSamplesIndex::CategoryName(), _ParticlesSamplesIndex()},
    // Procedural Textures Library samples
    {_ProceduralTexturesLibrarySamplesIndex::CategoryName(),
     _ProceduralTexturesLibrarySamplesIndex()},
    // Shadows samples
    {_ShadowsSamplesIndex::CategoryName(), _ShadowsSamplesIndex()},
    // Special FX samples
    {_SpecialFXSamplesIndex::CategoryName(), _SpecialFXSamplesIndex()},
    // Textures samples
    {_TexturesSamplesIndex::CategoryName(), _TexturesSamplesIndex()},
  };
}

SamplesIndex::~SamplesIndex()
{
}

bool SamplesIndex::isSampleEnabled(const std::string& sampleName) const
{
  for (const auto& item : _samplesIndex) {
    if (stl_util::contains(item.second.samples(), sampleName)) {
      return std::get<0>(item.second.samples().at(sampleName));
    }
  }

  return false;
}

std::optional<BABYLON::Samples::SampleFailureReason> SamplesIndex::doesSampleFail(const std::string& sampleName) const
{
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
  if (! is.good())
    is = std::ifstream("../samples_info.json");
  if (! is.good())
    is = std::ifstream("../../samples_info.json");
  if (is.good())
  {
    nlohmann::json j;
    is >> j;
    return j;
  }
  else
    return nlohmann::json();
}

SampleInfo SamplesIndex::getSampleInfo(const std::string& sampleName) const
{
  static nlohmann::json _samplesInfo = ReadSampleInfoFile();
  static std::map<std::string, SampleInfo> cache;
  if (cache.find(sampleName) != cache.end())
    return cache.at(sampleName);

  SampleInfo result;
  for (const auto & element : _samplesInfo)
  {
    if (element["sample_name"] == sampleName)
    {
      result.Brief = element["brief"];
      result.HeaderFile = element["header_file"];
      result.SourceFile = element["source_file"];
      for (const auto & link : element["links"])
      {
        result.Links.push_back(link);
      }
    }
  }
  cache[sampleName] = result;
  return result;
}

std::vector<std::string> SamplesIndex::getSampleNames() const
{
  // Extract the enabled sample names from the map
  std::vector<std::string> sampleNames;
  for (const auto& samplesCategory : _samplesIndex) {
    for (const auto& element : samplesCategory.second.samples()) {
      // Check if enabled
      if (std::get<0>(element.second)) {
        sampleNames.emplace_back(element.first);
      }
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
      // Check if enabled
      if (std::get<0>(element.second)) {
        sampleNames.emplace_back(element.first);
      }
    }
  }

  // Sort the vector with sample names in ascending order
  std::sort(sampleNames.begin(), sampleNames.end());

  return sampleNames;
}

IRenderableScenePtr
SamplesIndex::createRenderableScene(const std::string& sampleName,
                                    ICanvas* iCanvas) const
{
  for (const auto& item : _samplesIndex) {
    if (stl_util::contains(item.second.samples(), sampleName)) {
      return std::get<1>(item.second.samples().at(sampleName))(iCanvas);
    }
  }

  return nullptr;
}

void SamplesIndex::listSamples()
{
  auto categories = getCategoryNames();
  for (const auto & category : categories)
  {
    std::cout << "********************************************" << "\n";
    std::cout << "Category: " << category << "\n";
    std::cout << "********************************************" << "\n";
    auto samples = getSampleNamesInCategory(category);
    for (const auto & sample : samples) 
    {
      std::string ko = (!isSampleEnabled(sample)) ? " (KO)" : "";
      std::cout << sample << ko << "\n";
    }
  }
}

std::string SampleFailureReason_Str(SampleFailureReason s)
{
  switch (s) {
    case SampleFailureReason::blankDisplay:
      return "Blank display";
      break;
    case SampleFailureReason::outOfBoundAccess :
      return "Out of bounds access";
      break;
    case SampleFailureReason::processHung:
      return "Process hung";
      break;
    case SampleFailureReason::invalidComparator:
      return "Invalid comparator";
      break;
    case SampleFailureReason::vectorIteratorInvalid:
      return "vector iterators in range are from different containers";
      break;
    default:
      throw "Unhandled enum!";
  }
}

} // end of namespace Samples
} // end of namespace BABYLON
