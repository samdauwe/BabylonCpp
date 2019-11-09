#include <babylon/samples/materials/_materials_samples_index.h>

// Materials examples
#include <babylon/samples/materials/fresnel_parameters_torus_knot_scene.h>
#include <babylon/samples/materials/glossiness_and_roughness_scene.h>
#include <babylon/samples/materials/materials_scene.h>
#include <babylon/samples/materials/multi_material_scene.h>
#include <babylon/samples/materials/pbr_material_checker_orm_scene.h>
#include <babylon/samples/materials/pbr_material_orm_wood_scene.h>
#include <babylon/samples/materials/pbr_material_scene.h>
#include <babylon/samples/materials/pbr_metallic_roughness_gold_material_scene.h>
#include <babylon/samples/materials/pbr_metallic_roughness_material_scene.h>
#include <babylon/samples/materials/pbr_metallic_roughness_texture_material_scene.h>
#include <babylon/samples/materials/pbr_reflection_scene.h>
#include <babylon/samples/materials/pbr_specular_glossiness_material_scene.h>
#include <babylon/samples/materials/pbr_specular_glossiness_texture_material_scene.h>
#include <babylon/samples/materials/x_ray_material_with_fresnel_skull_scene.h>

// Shader material examples

namespace BABYLON {
namespace Samples {

_MaterialsSamplesIndex::_MaterialsSamplesIndex()
{
  // -- Materials examples --

  // Fresnel Parameters TorusKnot Scene
  _samples["FresnelParametersTorusKnotScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<FresnelParametersTorusKnotScene>(iCanvas);
  };
  // Glossiness and roughness scene
  _samples["GlossinessAndRoughnessScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<GlossinessAndRoughnessScene>(iCanvas);
  };
  // Materials Scene
  _samples["MaterialsScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<MaterialsScene>(iCanvas);
  };
  // Multi Material Scene
  _samples["MultiMaterialScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<MultiMaterialScene>(iCanvas);
  };
  // PBR Material Checker ORM Scene
  _samples["PBRMaterialCheckerORMScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<PBRMaterialCheckerORMScene>(iCanvas);
  };
  // PBR Material ORM Wood Scene
  _samples["PBRMaterialORMWoodScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<PBRMaterialORMWoodScene>(iCanvas);
  };
  // PBR Material Scene
  _samples["PBRMaterialScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<PBRMaterialScene>(iCanvas);
  };
  // PBR Metallic Roughness Gold Material
  _samples["PBRMetallicRoughnessGoldMaterialScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<PBRMetallicRoughnessGoldMaterialScene>(iCanvas);
  };
  // PBR Metallic Roughness Material
  _samples["PBRMetallicRoughnessMaterialScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<PBRMetallicRoughnessMaterialScene>(iCanvas);
  };
  // PBR Metallic Roughness Texture Material Scene
  _samples["PBRMetallicRoughnessTextureMaterialScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<PBRMetallicRoughnessTextureMaterialScene>(iCanvas);
  };
  // PBR Reflection Scene
  _samples["PBRReflectionScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<PBRReflectionScene>(iCanvas);
  };
  // PBR Specular Glossiness Material Scene
  _samples["PBRSpecularGlossinessMaterialScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<PBRSpecularGlossinessMaterialScene>(iCanvas);
  };
  // PBR Specular Glossiness Texture Material Scene
  _samples["PBRSpecularGlossinessTextureMaterialScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<PBRSpecularGlossinessTextureMaterialScene>(iCanvas);
  };
  // X-ray Material with Fresnel Skull Scene
  _samples["XRayMaterialWithFresnelSkullScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<XRayMaterialWithFresnelSkullScene>(iCanvas);
  };

}
} // end of namespace Samples
} // end of namespace BABYLON