#include <babylon/samples/samples_index.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/interfaces/irenderable_scene.h>
#include <babylon/samples/animations/_animations_samples_index.h>
#include <babylon/samples/cameras/_cameras_samples_index.h>
#include <babylon/samples/extensions/_extensions_samples_index.h>
#include <babylon/samples/lights/_lights_samples_index.h>
#include <babylon/samples/loaders/_loaders_samples_index.h>
#include <babylon/samples/materials/_materials_samples_index.h>
#include <babylon/samples/meshes/_meshes_samples_index.h>
#include <babylon/samples/optimizations/_optimizations_samples_index.h>
#include <babylon/samples/specialfx/_special_fx_samples_index.h>
#include <babylon/samples/textures/_textures_samples_index.h>

namespace BABYLON {
namespace Samples {

SamplesIndex::SamplesIndex()
{
  // Inintialize the samples index
  _samplesIndex = {
    // Animations samples
    {_AnimationsSamplesIndex::CategoryName(), _AnimationsSamplesIndex()},
    // Cameras samples
    {_CamerasSamplesIndex::CategoryName(), _CamerasSamplesIndex()},
    // Extensions samples
    {_ExtensionsSamplesIndex::CategoryName(), _ExtensionsSamplesIndex()},
    // Lights samples
    {_LightsSamplesIndex::CategoryName(), _LightsSamplesIndex()},
    // Loaders samples
    {_LoadersSamplesIndex::CategoryName(), _LoadersSamplesIndex()},
    // Materials samples
    {_MaterialsSamplesIndex::CategoryName(), _MaterialsSamplesIndex()},
    // Meshes samples
    {_MeshesSamplesIndex::CategoryName(), _MeshesSamplesIndex()},
    // Optimizations samples
    {_OptimizationsSamplesIndex::CategoryName(), _OptimizationsSamplesIndex()},
    // Special FX samples
    {_SpecialFXSamplesIndex::CategoryName(), _SpecialFXSamplesIndex()},
    // Textures samples
    {_TexturesSamplesIndex::CategoryName(), _TexturesSamplesIndex()},
  };
}

SamplesIndex::~SamplesIndex()
{
}

bool SamplesIndex::isSampleEnabled(const string_t& sampleName)
{
  for (const auto& item : _samplesIndex) {
    if (stl_util::contains(item.second.samples(), sampleName)) {
      return std::get<0>(item.second.samples().at(sampleName));
    }
  }

  return false;
}

bool SamplesIndex::sampleExists(const string_t& sampleName)
{
  for (const auto& item : _samplesIndex) {
    if (stl_util::contains(item.second.samples(), sampleName)) {
      return true;
    }
  }

  return false;
}

::std::vector<string_t> SamplesIndex::getCategoryNames() const
{
  // Extract the categpry names
  auto categoryNames = stl_util::extract_keys(_samplesIndex);

  // Sort the vector with category names in ascending order
  std::sort(categoryNames.begin(), categoryNames.end());

  return categoryNames;
}

std::vector<string_t> SamplesIndex::getSampleNames() const
{
  // Extract the enabled sample names from the map
  std::vector<string_t> sampleNames;
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

::std::vector<string_t>
SamplesIndex::getSampleNamesInCategory(const string_t& categoryName) const
{
  // Extract the enabled sample names for the given category from the map
  std::vector<string_t> sampleNames;
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

std::unique_ptr<IRenderableScene>
SamplesIndex::createRenderableScene(const string_t& sampleName,
                                    ICanvas* iCanvas) const
{
  for (const auto& item : _samplesIndex) {
    if (stl_util::contains(item.second.samples(), sampleName)) {
      return std::get<1>(item.second.samples().at(sampleName))(iCanvas);
    }
  }

  return nullptr;
}

} // end of namespace Samples
} // end of namespace BABYLON
