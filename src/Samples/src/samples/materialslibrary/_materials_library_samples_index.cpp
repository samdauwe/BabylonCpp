#include <babylon/samples/materialslibrary/_materials_library_samples_index.h>

#include <babylon/samples/materialslibrary/cell_material_scene.h>
#include <babylon/samples/materialslibrary/cell_shading_material_scene.h>
#include <babylon/samples/materialslibrary/fire_material_scene.h>
#include <babylon/samples/materialslibrary/fur_material_scene.h>
#include <babylon/samples/materialslibrary/gradient_material_scene.h>
#include <babylon/samples/materialslibrary/gradient_material_sphere_scene.h>
#include <babylon/samples/materialslibrary/grid_material_scene.h>
#include <babylon/samples/materialslibrary/lava_material_scene.h>
#include <babylon/samples/materialslibrary/mix_material_scene.h>
#include <babylon/samples/materialslibrary/normal_material_scene.h>
#include <babylon/samples/materialslibrary/simple_material_scene.h>
#include <babylon/samples/materialslibrary/sky_material_scene.h>
#include <babylon/samples/materialslibrary/terrain_material_scene.h>
#include <babylon/samples/materialslibrary/tri_planar_material_scene.h>
#include <babylon/samples/materialslibrary/water_material_scene.h>

namespace BABYLON {
namespace Samples {

_MaterialsLibrarySamplesIndex::_MaterialsLibrarySamplesIndex()
{
  // -- Materials Library examples --

  // Cell Material Scene
  _samples["CellMaterialScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<CellMaterialScene>(iCanvas);
  };
  // Cell Shading Material Scene
  _samples["CellShadingMaterialScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<CellShadingMaterialScene>(iCanvas);
  };
  // Fire Material Scene
  _samples["FireMaterialScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<FireMaterialScene>(iCanvas);
  };
  // Fur Material Scene
  _samples["FurMaterialScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<FurMaterialScene>(iCanvas);
  };
  // Gradient Material Scene
  _samples["GradientMaterialScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<GradientMaterialScene>(iCanvas);
  };
  // Gradient Material Sphere Scene
  _samples["GradientMaterialSphereScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<GradientMaterialSphereScene>(iCanvas);
  };
  // Grid Material Scene
  _samples["GridMaterialScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<GridMaterialScene>(iCanvas);
  };
  // Lava Material Scene
  _samples["LavaMaterialScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<LavaMaterialScene>(iCanvas);
  };
  // Mix Material Scene
  _samples["MixMaterialScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<MixMaterialScene>(iCanvas);
  };
  // Normal Material Scene
  _samples["NormalMaterialScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<NormalMaterialScene>(iCanvas);
  };
  // Simple Material Scene
  _samples["SimpleMaterialScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<SimpleMaterialScene>(iCanvas);
  };
  // Sky Material Scene
  _samples["SkyMaterialScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<SkyMaterialScene>(iCanvas);
  };
  // Terrain Material Scene
  _samples["TerrainMaterialScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<TerrainMaterialScene>(iCanvas);
  };
  // Tri-Planar Material Scene
  _samples["TriPlanarMaterialScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<TriPlanarMaterialScene>(iCanvas);
  };
  // Water Material Scene
  _samples["WaterMaterialScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<WaterMaterialScene>(iCanvas);
  };
}
} // end of namespace Samples
} // end of namespace BABYLON
