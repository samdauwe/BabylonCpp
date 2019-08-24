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
  {"ColoredRibbonScene", SampleFailureReason::outOfBoundAccess},
  {"CurvedHelixMeshesScene", SampleFailureReason::outOfBoundAccess},
  {"ExtrusionScene", SampleFailureReason::outOfBoundAccess},
  {"MirrorTextureScene", SampleFailureReason::readAccessViolation},

  // Empty 3d
  {"BlurModeForMirrorsScene", SampleFailureReason::empty3d},
  {"BoneRotationInWorldSpaceScene", SampleFailureReason::empty3d},
  {"BoxShadowScene", SampleFailureReason::empty3d},
  {"BrickProceduralTextureScene", SampleFailureReason::empty3d},
  {"BulbSelfShadowScene", SampleFailureReason::empty3d},
  {"CloudProceduralTextureScene", SampleFailureReason::empty3d},
  {"ConvolutionPostProcessScene", SampleFailureReason::empty3d},
  {"CustomRenderTargetsScene", SampleFailureReason::empty3d},
  {"DawnBringerScene", SampleFailureReason::empty3d},
  {"FadeInOutScene", SampleFailureReason::empty3d},
  {"FireProceduralTextureScene", SampleFailureReason::empty3d},
  {"GrassProceduralTextureScene", SampleFailureReason::empty3d},
  {"HeatWaveScene", SampleFailureReason::empty3d},
  {"HighlightLayerScene", SampleFailureReason::empty3d},
  {"ImportDudeScene", SampleFailureReason::empty3d},
  {"ImportDummy3Scene", SampleFailureReason::empty3d},
  {"ImportRabbitScene", SampleFailureReason::empty3d},
  {"KernelBasedBlurScene", SampleFailureReason::empty3d},
  {"MarbleProceduralTextureScene", SampleFailureReason::empty3d},
  {"MirrorsScene", SampleFailureReason::empty3d},
  {"MultiSampleRenderTargetsScene", SampleFailureReason::empty3d},
  {"PBRMaterialScene", SampleFailureReason::empty3d},
  {"PBRMetallicRoughnessMaterialScene", SampleFailureReason::empty3d},
  {"PBRReflectionScene", SampleFailureReason::empty3d},
  {"PerlinNoiseProceduralTextureScene", SampleFailureReason::empty3d},
  {"PortalsScene", SampleFailureReason::empty3d},
  {"ProceduralTexturesScene", SampleFailureReason::empty3d},
  {"RealtimeRefractionScene", SampleFailureReason::empty3d},
  {"RenderTargetTextureScene", SampleFailureReason::empty3d},
  {"RGBShiftGlitchScene", SampleFailureReason::empty3d},
  {"RoadProceduralTextureScene", SampleFailureReason::empty3d},
  {"ShaderMaterialWarpSpeedScene", SampleFailureReason::empty3d},
  {"ShadowsScene", SampleFailureReason::empty3d},
  {"SimplePostProcessRenderPipelineScene", SampleFailureReason::empty3d},
  {"StarfieldProceduralTextureScene", SampleFailureReason::empty3d},
  {"WaterMaterialScene", SampleFailureReason::empty3d},
  {"WoodProceduralTextureScene", SampleFailureReason::empty3d},

  {"MorphTargetsScene", SampleFailureReason::incomplete3d},
  {"PBRMaterialCheckerORMScene", SampleFailureReason::incomplete3d},
  {"PBRMaterialORMWoodScene", SampleFailureReason::incomplete3d},
  {"PBRMetallicRoughnessGoldMaterialScene", SampleFailureReason::incomplete3d},
  {"PBRMetallicRoughnessTextureMaterialScene", SampleFailureReason::incomplete3d},

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
    case SampleFailureReason::empty3d:
      return "3D rendering is empty";
      break;
    case SampleFailureReason::readAccessViolation:
      return "Read Access Violation";
      break;
    case SampleFailureReason::incomplete3d:
      return "Incomplete 3d (black textures and co)";
      break;
    default:
      throw "Unhandled enum!";
  }
}

} // end of namespace Samples
} // end of namespace BABYLON
