#include <babylon/samples/materials/_materials_samples_index.h>

#include <babylon/samples/materials/fresnel_parameters_torus_knot_scene.h>
#include <babylon/samples/materials/glossiness_and_roughness_scene.h>
#include <babylon/samples/materials/materials_scene.h>
#include <babylon/samples/materials/multi_material_scene.h>
#include <babylon/samples/materials/pbr_material_scene.h>
#include <babylon/samples/materials/pbr_metallic_roughness_gold_material_scene.h>
#include <babylon/samples/materials/pbr_metallic_roughness_material_scene.h>
#include <babylon/samples/materials/pbr_metallic_roughness_texture_material_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_box_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_boxes_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_carved_trees_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_chains_and_gears_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_clouds_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_cylinders_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_double_helix_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_generators_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_kifs_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_luminescence_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_menger_sponge_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_morning_city_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_opera_island_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_pbr_test_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_pegasus_galaxy_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_physically_based_sdf_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_planet_shadertoy_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_seascape_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_server_room_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_simplicity_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_skybox_clouds_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_skybox_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_sparse_grid_marching_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_speed_tracer_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_star_nest_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_the_drive_home_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_tiles_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_voronoian_waterway_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_warp_speed_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_with_fog_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_wolfenstein_3d_scene.h>
#include <babylon/samples/materials/shadermaterial/shader_material_xyptonjtroz_scene.h>
#include <babylon/samples/materials/x_ray_material_with_fresnel_skull_scene.h>

namespace BABYLON {
namespace Samples {

_MaterialsSamplesIndex::_MaterialsSamplesIndex()
{
  // -- Materials examples --

  // Fresnel Parameters TorusKnot Scene
  _samples["FresnelParametersTorusKnotScene"] = std::make_tuple(
    true,                                                                //
    [](ICanvas* iCanvas) {                                               //
      return std::make_unique<FresnelParametersTorusKnotScene>(iCanvas); //
    });                                                                  //
  // Glossiness and roughness scene
  _samples["GlossinessAndRoughnessScene"] = std::make_tuple(
    true,                                                            //
    [](ICanvas* iCanvas) {                                           //
      return std::make_unique<GlossinessAndRoughnessScene>(iCanvas); //
    });                                                              //
  // Materials Scene
  _samples["MaterialsScene"]
    = std::make_tuple(true,                                               //
                      [](ICanvas* iCanvas) {                              //
                        return std::make_unique<MaterialsScene>(iCanvas); //
                      });                                                 //
  // Multi Material Scene
  _samples["MultiMaterialScene"]
    = std::make_tuple(true,                                                   //
                      [](ICanvas* iCanvas) {                                  //
                        return std::make_unique<MultiMaterialScene>(iCanvas); //
                      });                                                     //
  // PBR Material Scenee
  _samples["PBRMaterialScene"]
    = std::make_tuple(true,                                                 //
                      [](ICanvas* iCanvas) {                                //
                        return std::make_unique<PBRMaterialScene>(iCanvas); //
                      });                                                   //
  // PBR Metallic Roughness Gold Material
  _samples["PBRMetallicRoughnessGoldMaterialScene"] = std::make_tuple(
    false,                 //
    [](ICanvas* iCanvas) { //
      return std::make_unique<PBRMetallicRoughnessGoldMaterialScene>(
        iCanvas); //
    });           //
  // PBR Metallic Roughness Material
  _samples["PBRMetallicRoughnessMaterialScene"] = std::make_tuple(
    false,                                                                 //
    [](ICanvas* iCanvas) {                                                 //
      return std::make_unique<PBRMetallicRoughnessMaterialScene>(iCanvas); //
    });                                                                    //
  // PBR Metallic Roughness Texture Material Scene
  _samples["PBRMetallicRoughnessTextureMaterialScene"] = std::make_tuple(
    false,                 //
    [](ICanvas* iCanvas) { //
      return std::make_unique<PBRMetallicRoughnessTextureMaterialScene>(
        iCanvas); //
    });           //
  // X-ray Material with Fresnel Skull Scene
  _samples["XRayMaterialWithFresnelSkullScene"] = std::make_tuple(
    true,                                                                  //
    [](ICanvas* iCanvas) {                                                 //
      return std::make_unique<XRayMaterialWithFresnelSkullScene>(iCanvas); //
    });                                                                    //

  // -- Shader material examples --

  // Shader Material Box Scene
  _samples["ShaderMaterialBoxScene"] = std::make_tuple(
    true,                                                       //
    [](ICanvas* iCanvas) {                                      //
      return std::make_unique<ShaderMaterialBoxScene>(iCanvas); //
    });                                                         //
  // Shader Material Boxes Scene
  _samples["ShaderMaterialBoxesScene"] = std::make_tuple(
    true,                                                         //
    [](ICanvas* iCanvas) {                                        //
      return std::make_unique<ShaderMaterialBoxesScene>(iCanvas); //
    });                                                           //
  // Shader Material Carved Trees Scene
  _samples["ShaderMaterialCarvedTreesScene"] = std::make_tuple(
    true,                                                               //
    [](ICanvas* iCanvas) {                                              //
      return std::make_unique<ShaderMaterialCarvedTreesScene>(iCanvas); //
    });                                                                 //
  // Shader Material Chains and Gears Scene
  _samples["ShaderMaterialChainsAndGearsScene"] = std::make_tuple(
    true,                                                                  //
    [](ICanvas* iCanvas) {                                                 //
      return std::make_unique<ShaderMaterialChainsAndGearsScene>(iCanvas); //
    });                                                                    //
  // Shader Material Clouds Scene
  _samples["ShaderMaterialCloudsScene"] = std::make_tuple(
    true,                                                          //
    [](ICanvas* iCanvas) {                                         //
      return std::make_unique<ShaderMaterialCloudsScene>(iCanvas); //
    });                                                            //
  // Shader Material Cylinders Scene
  _samples["ShaderMaterialCylindersScene"] = std::make_tuple(
    true,                                                             //
    [](ICanvas* iCanvas) {                                            //
      return std::make_unique<ShaderMaterialCylindersScene>(iCanvas); //
    });                                                               //
  // Shader Material Double Helix Scene
  _samples["ShaderMaterialDoubleHelixScene"] = std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                              //
      return std::make_unique<ShaderMaterialDoubleHelixScene>(iCanvas); //
    });                                                                 //
  // Shader Material Generators Scene
  _samples["ShaderMaterialGeneratorsScene"] = std::make_tuple(
    true,                                                              //
    [](ICanvas* iCanvas) {                                             //
      return std::make_unique<ShaderMaterialGeneratorsScene>(iCanvas); //
    });                                                                //
  // Shader Material KIFS Scene
  _samples["ShaderMaterialKIFSScene"] = std::make_tuple(
    true,                                                        //
    [](ICanvas* iCanvas) {                                       //
      return std::make_unique<ShaderMaterialKIFSScene>(iCanvas); //
    });                                                          //
  // Shader Material Luminescence Scene
  _samples["ShaderMaterialLuminescenceScene"] = std::make_tuple(
    true,                                                                //
    [](ICanvas* iCanvas) {                                               //
      return std::make_unique<ShaderMaterialLuminescenceScene>(iCanvas); //
    });                                                                  //
  // Shader Material Menger Sponge Scene
  _samples["ShaderMaterialMengerSpongeScene"] = std::make_tuple(
    true,                                                                //
    [](ICanvas* iCanvas) {                                               //
      return std::make_unique<ShaderMaterialMengerSpongeScene>(iCanvas); //
    });                                                                  //
  // Shader Material Morning City Scene
  _samples["ShaderMaterialMorningCityScene"] = std::make_tuple(
    true,                                                               //
    [](ICanvas* iCanvas) {                                              //
      return std::make_unique<ShaderMaterialMorningCityScene>(iCanvas); //
    });                                                                 //
  // Shader Material Opera Island Scene
  _samples["ShaderMaterialOperaIslandScene"] = std::make_tuple(
    true,                                                               //
    [](ICanvas* iCanvas) {                                              //
      return std::make_unique<ShaderMaterialOperaIslandScene>(iCanvas); //
    });                                                                 //
  // Shader Material PBR Test Scene
  _samples["ShaderMaterialPBRTestScene"] = std::make_tuple(
    true,                                                           //
    [](ICanvas* iCanvas) {                                          //
      return std::make_unique<ShaderMaterialPBRTestScene>(iCanvas); //
    });                                                             //
  // Shader Material Pegasus Galaxy Scene
  _samples["ShaderMaterialPegasusGalaxyScene"] = std::make_tuple(
    true,                                                                 //
    [](ICanvas* iCanvas) {                                                //
      return std::make_unique<ShaderMaterialPegasusGalaxyScene>(iCanvas); //
    });                                                                   //
  // Shader Material Physically-based SDF Scene
  _samples["ShaderMaterialPhysicallyBasedSDFScene"] = std::make_tuple(
    true,                                                             //
    [](ICanvas* iCanvas) {                                            //
      return std::make_unique<ShaderMaterialPhysicallyBasedSDFScene>( //
        iCanvas);                                                     //
    });                                                               //
  // Shader Material Planet Shadertoy Scene
  _samples["ShaderMaterialPlanetShadertoyScene"] = std::make_tuple(
    true,                                                                   //
    [](ICanvas* iCanvas) {                                                  //
      return std::make_unique<ShaderMaterialPlanetShadertoyScene>(iCanvas); //
    });                                                                     //
  // Shader Material Scene
  _samples["ShaderMaterialScene"] = std::make_tuple(
    true,                                                    //
    [](ICanvas* iCanvas) {                                   //
      return std::make_unique<ShaderMaterialScene>(iCanvas); //
    });                                                      //
  // Shader Material Seascape Scene
  _samples["ShaderMaterialSeascapeScene"] = std::make_tuple(
    true,                                                            //
    [](ICanvas* iCanvas) {                                           //
      return std::make_unique<ShaderMaterialSeascapeScene>(iCanvas); //
    });                                                              //
  // Shader Material Server Room Scene
  _samples["ShaderMaterialServerRoomScene"] = std::make_tuple(
    true,                                                              //
    [](ICanvas* iCanvas) {                                             //
      return std::make_unique<ShaderMaterialServerRoomScene>(iCanvas); //
    });                                                                //
  // Shader Material Simplicity Scene
  _samples["ShaderMaterialSimplicityScene"] = std::make_tuple(
    true,                                                              //
    [](ICanvas* iCanvas) {                                             //
      return std::make_unique<ShaderMaterialSimplicityScene>(iCanvas); //
    });                                                                //
  // Shader Material Skybox Clouds Scene
  _samples["ShaderMaterialSkyboxCloudsScene"] = std::make_tuple(
    true,                                                                //
    [](ICanvas* iCanvas) {                                               //
      return std::make_unique<ShaderMaterialSkyboxCloudsScene>(iCanvas); //
    });                                                                  //
  // Shader Material Sparse Grid Marching Scene
  _samples["ShaderMaterialSparseGridMarchingScene"] = std::make_tuple(
    true,                                                             //
    [](ICanvas* iCanvas) {                                            //
      return std::make_unique<ShaderMaterialSparseGridMarchingScene>( //
        iCanvas);                                                     //
    });                                                               //
  // Shader Material Speed Tracer Scene
  _samples["ShaderMaterialSpeedTracerScene"] = std::make_tuple(
    true,                                                               //
    [](ICanvas* iCanvas) {                                              //
      return std::make_unique<ShaderMaterialSpeedTracerScene>(iCanvas); //
    });                                                                 //
  // Shader Material Skybox Scene
  _samples["ShaderMaterialSkyboxScene"] = std::make_tuple(
    true,                                                          //
    [](ICanvas* iCanvas) {                                         //
      return std::make_unique<ShaderMaterialSkyboxScene>(iCanvas); //
    });                                                            //
  // Shader Material Star Nest Scene
  _samples["ShaderMaterialStarNestScene"] = std::make_tuple(
    true,                                                            //
    [](ICanvas* iCanvas) {                                           //
      return std::make_unique<ShaderMaterialStarNestScene>(iCanvas); //
    });                                                              //
  // Shader Material The Drive Home Scene
  _samples["ShaderMaterialTheDriveHomeScene"] = std::make_tuple(
    true,                                                                //
    [](ICanvas* iCanvas) {                                               //
      return std::make_unique<ShaderMaterialTheDriveHomeScene>(iCanvas); //
    });                                                                  //
  // Shader Material Tiles Scene
  _samples["ShaderMaterialTilesScene"] = std::make_tuple(
    true,                                                         //
    [](ICanvas* iCanvas) {                                        //
      return std::make_unique<ShaderMaterialTilesScene>(iCanvas); //
    });                                                           //
  // Shader Material Voronoian Waterway Scene
  _samples["ShaderMaterialVoronoianWaterwayScene"] = std::make_tuple(
    true,                                                            //
    [](ICanvas* iCanvas) {                                           //
      return std::make_unique<ShaderMaterialVoronoianWaterwayScene>( //
        iCanvas);                                                    //
    });                                                              //
  // Shader Material Warp Speed Scene
  _samples["ShaderMaterialWarpSpeedScene"] = std::make_tuple(
    true,                                                    //
    [](ICanvas* iCanvas) {                                   //
      return std::make_unique<ShaderMaterialWarpSpeedScene>( //
        iCanvas);                                            //
    });                                                      //
  // Shader Material With Fog Scene
  _samples["ShaderMaterialWithFogScene"] = std::make_tuple(
    true,                                                           //
    [](ICanvas* iCanvas) {                                          //
      return std::make_unique<ShaderMaterialWithFogScene>(iCanvas); //
    });                                                             //
  // Shader Material Wolfenstein 3D Scene
  _samples["ShaderMaterialWolfenstein3DScene"] = std::make_tuple(
    true,                                                                 //
    [](ICanvas* iCanvas) {                                                //
      return std::make_unique<ShaderMaterialWolfenstein3DScene>(iCanvas); //
    });                                                                   //
  // Shader Material Xyptonjtroz Scene
  _samples["ShaderMaterialXyptonjtrozScene"] = std::make_tuple(
    true,                                                               //
    [](ICanvas* iCanvas) {                                              //
      return std::make_unique<ShaderMaterialXyptonjtrozScene>(iCanvas); //
    });                                                                 //
}

_MaterialsSamplesIndex::~_MaterialsSamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
