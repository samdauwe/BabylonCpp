#include <babylon/samples/samples_index.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/samples/animations/animations_scene.h>
#include <babylon/samples/animations/cartoon_animations_scene.h>
#include <babylon/samples/animations/easing/easing_functions_scene.h>
#include <babylon/samples/animations/easing/shape_easings_scene.h>
#include <babylon/samples/animations/pump_jack_scene.h>
#include <babylon/samples/animations/tube_animation_scene.h>
#include <babylon/samples/effects/fog_scene.h>
#include <babylon/samples/environment/height_map_scene.h>
#include <babylon/samples/extensions/hexplanetgeneration/hex_ico_sphere_scene.h>
#include <babylon/samples/extensions/hexplanetgeneration/procedural_hex_planet_generation_scene.h>
#include <babylon/samples/extensions/navigation/crowd_simulation_scene_01.h>
#include <babylon/samples/extensions/navigation/crowd_simulation_scene_02.h>
#include <babylon/samples/extensions/navigation/crowd_simulation_scene_03.h>
#include <babylon/samples/extensions/navigation/crowd_simulation_scene_04.h>
#include <babylon/samples/extensions/navigation/crowd_simulation_scene_05.h>
#include <babylon/samples/extensions/navigation/crowd_simulation_scene_06.h>
#include <babylon/samples/extensions/navigation/crowd_simulation_scene_07.h>
#include <babylon/samples/extensions/navigation/crowd_simulation_scene_08.h>
#include <babylon/samples/extensions/navigation/crowd_simulation_scene_09.h>
#include <babylon/samples/extensions/navigation/crowd_simulation_scene_10.h>
#include <babylon/samples/extensions/navigation/crowd_simulation_scene_11.h>
#include <babylon/samples/extensions/noisegeneration/perlin_noise_waves_scene.h>
#include <babylon/samples/extensions/polyhedron/polyhedra_scene.h>
#include <babylon/samples/extensions/treegenerators/simple_oak_tree_scene.h>
#include <babylon/samples/extensions/treegenerators/simple_pine_scene.h>
#include <babylon/samples/generic/fireworks_with_shader_code_scene.h>
#include <babylon/samples/generic/house_from_floorplan.h>
#include <babylon/samples/generic/infinite_loader_scene.h>
#include <babylon/samples/generic/lorenz_attractor_scene.h>
#include <babylon/samples/generic/mandelbrot_fractal_scene.h>
#include <babylon/samples/generic/simple_car_following_path_scene.h>
#include <babylon/samples/generic/super_ellipsoid_scene.h>
#include <babylon/samples/generic/waves_scene.h>
#include <babylon/samples/lights/lights_scene.h>
#include <babylon/samples/materials/fresnel_parameters_torus_knot_scene.h>
#include <babylon/samples/materials/shader_material_box_scene.h>
#include <babylon/samples/materials/shader_material_boxes_scene.h>
#include <babylon/samples/materials/shader_material_carved_trees_scene.h>
#include <babylon/samples/materials/shader_material_chains_and_gears_scene.h>
#include <babylon/samples/materials/shader_material_cylinders_scene.h>
#include <babylon/samples/materials/shader_material_double_helix_scene.h>
#include <babylon/samples/materials/shader_material_generators_scene.h>
#include <babylon/samples/materials/shader_material_kifs_scene.h>
#include <babylon/samples/materials/shader_material_luminescence_scene.h>
#include <babylon/samples/materials/shader_material_menger_sponge_scene.h>
#include <babylon/samples/materials/shader_material_morning_city_scene.h>
#include <babylon/samples/materials/shader_material_pbr_test_scene.h>
#include <babylon/samples/materials/shader_material_pegasus_galaxy_scene.h>
#include <babylon/samples/materials/shader_material_physically_based_sdf_scene.h>
#include <babylon/samples/materials/shader_material_planet_shadertoy_scene.h>
#include <babylon/samples/materials/shader_material_scene.h>
#include <babylon/samples/materials/shader_material_seascape_scene.h>
#include <babylon/samples/materials/shader_material_simplicity_scene.h>
#include <babylon/samples/materials/shader_material_skybox_clouds_scene.h>
#include <babylon/samples/materials/shader_material_skybox_scene.h>
#include <babylon/samples/materials/shader_material_sparse_grid_marching_scene.h>
#include <babylon/samples/materials/shader_material_speed_tracer_scene.h>
#include <babylon/samples/materials/shader_material_star_nest_scene.h>
#include <babylon/samples/materials/shader_material_the_drive_home_scene.h>
#include <babylon/samples/materials/shader_material_the_road_to_hell.h>
#include <babylon/samples/materials/shader_material_tiles_scene.h>
#include <babylon/samples/materials/shader_material_voronoian_waterway_scene.h>
#include <babylon/samples/materials/shader_material_with_fog_scene.h>
#include <babylon/samples/materials/shader_material_wolfenstein_3d_scene.h>
#include <babylon/samples/materials/shader_material_xyptonjtroz_scene.h>
#include <babylon/samples/mesh/basic_elements_scene.h>
#include <babylon/samples/mesh/basic_scene.h>
#include <babylon/samples/mesh/circle_curves_from_beziers.h>
#include <babylon/samples/mesh/cuboid_scene.h>
#include <babylon/samples/mesh/curved_helix_meshes_scene.h>
#include <babylon/samples/mesh/extruded_polygon_scene.h>
#include <babylon/samples/mesh/extrusion_scene.h>
#include <babylon/samples/mesh/ico_sphere_scene.h>
#include <babylon/samples/mesh/lathe_scene.h>
#include <babylon/samples/mesh/lines_mesh_spiral_scene.h>
#include <babylon/samples/mesh/merged_meshes_scene.h>
#include <babylon/samples/mesh/mesh_normals_scene.h>
#include <babylon/samples/mesh/polygonmesh/polygon_mesh_bird_scene.h>
#include <babylon/samples/mesh/polygonmesh/polygon_mesh_monkey_scene.h>
#include <babylon/samples/mesh/polygonmesh/polygon_mesh_puzzle_piece_scene.h>
#include <babylon/samples/mesh/polygonmesh/polygon_mesh_stars_scene.h>
#include <babylon/samples/mesh/polygonmesh/polygon_mesh_stars_with_holes_scene.h>
#include <babylon/samples/mesh/polyhedron_scene.h>
#include <babylon/samples/mesh/ribbon_scene.h>
#include <babylon/samples/mesh/rotation_and_scaling_scene.h>
#include <babylon/samples/mesh/tiled_ground_scene.h>
#include <babylon/samples/mesh/tube_scene.h>

namespace BABYLON {
namespace Samples {

SamplesIndex::SamplesIndex()
{
  // Animations Scene
  _samples["AnimationsScene"]
    = ::std::make_tuple(true, [](ICanvas* iCanvas) {         //
        return ::std::make_unique<AnimationsScene>(iCanvas); //
      });
  // Animations Scene
  _samples["AnimationsScene"]
    = ::std::make_tuple(true, [](ICanvas* iCanvas) {         //
        return ::std::make_unique<AnimationsScene>(iCanvas); //
      });                                                    //
  // Basic Elements Scene
  _samples["BasicElementsScene"]
    = ::std::make_tuple(true, [](ICanvas* iCanvas) {            //
        return ::std::make_unique<BasicElementsScene>(iCanvas); //
      });                                                       //
  // Basic Scene
  _samples["BasicScene"] = ::std::make_tuple(true, [](ICanvas* iCanvas) { //
    return ::std::make_unique<BasicScene>(iCanvas);                       //
  });                                                                     //
  // Cartoon Animations Scene
  _samples["CartoonAnimationsScene"]
    = ::std::make_tuple(false, [](ICanvas* iCanvas) {               //
        return ::std::make_unique<CartoonAnimationsScene>(iCanvas); //
      });                                                           //
  // Circle Curves From Beziers Scene
  _samples["CircleCurvesFromBeziersScene"]
    = ::std::make_tuple(true, [](ICanvas* iCanvas) {                      //
        return ::std::make_unique<CircleCurvesFromBeziersScene>(iCanvas); //
      });                                                                 //
  // Crowd Simulation Scene 01
  _samples["CrowdSimulationScene01"]
    = ::std::make_tuple(true, [](ICanvas* iCanvas) {                //
        return ::std::make_unique<CrowdSimulationScene01>(iCanvas); //
      });                                                           //
  // Crowd Simulation Scene 02
  _samples["CrowdSimulationScene02"]
    = ::std::make_tuple(true, [](ICanvas* iCanvas) {                //
        return ::std::make_unique<CrowdSimulationScene02>(iCanvas); //
      });                                                           //
  // Crowd Simulation Scene 03
  _samples["CrowdSimulationScene03"]
    = ::std::make_tuple(true, [](ICanvas* iCanvas) {                //
        return ::std::make_unique<CrowdSimulationScene03>(iCanvas); //
      });                                                           //
  // Crowd Simulation Scene 04
  _samples["CrowdSimulationScene04"]
    = ::std::make_tuple(true, [](ICanvas* iCanvas) {                //
        return ::std::make_unique<CrowdSimulationScene04>(iCanvas); //
      });                                                           //
  // Crowd Simulation Scene 05
  _samples["CrowdSimulationScene05"]
    = ::std::make_tuple(true, [](ICanvas* iCanvas) {                //
        return ::std::make_unique<CrowdSimulationScene05>(iCanvas); //
      });                                                           //
  // Crowd Simulation Scene 06
  _samples["CrowdSimulationScene06"]
    = ::std::make_tuple(true, [](ICanvas* iCanvas) {                //
        return ::std::make_unique<CrowdSimulationScene06>(iCanvas); //
      });                                                           //
  // Crowd Simulation Scene 07
  _samples["CrowdSimulationScene07"]
    = ::std::make_tuple(true, [](ICanvas* iCanvas) {                //
        return ::std::make_unique<CrowdSimulationScene07>(iCanvas); //
      });                                                           //
  // Crowd Simulation Scene 08
  _samples["CrowdSimulationScene08"]
    = ::std::make_tuple(true, [](ICanvas* iCanvas) {                //
        return ::std::make_unique<CrowdSimulationScene08>(iCanvas); //
      });                                                           //
  // Crowd Simulation Scene 09
  _samples["CrowdSimulationScene09"]
    = ::std::make_tuple(true, [](ICanvas* iCanvas) {                //
        return ::std::make_unique<CrowdSimulationScene09>(iCanvas); //
      });                                                           //
  // Crowd Simulation Scene 10
  _samples["CrowdSimulationScene10"]
    = ::std::make_tuple(true, [](ICanvas* iCanvas) {                //
        return ::std::make_unique<CrowdSimulationScene10>(iCanvas); //
      });                                                           //
  // Crowd Simulation Scene 11
  _samples["CrowdSimulationScene11"]
    = ::std::make_tuple(true, [](ICanvas* iCanvas) {                //
        return ::std::make_unique<CrowdSimulationScene11>(iCanvas); //
      });                                                           //
  // Cuboid Scene
  _samples["CuboidScene"]
    = ::std::make_tuple(true,
                        [](ICanvas* iCanvas) {                             //
                          return ::std::make_unique<CuboidScene>(iCanvas); //
                        });                                                //
  // Curved Helix Meshes Scene
  _samples["CurvedHelixMeshesScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                        //
      return ::std::make_unique<CurvedHelixMeshesScene>(iCanvas); //
    });                                                           //
  // Easing Functions Scene
  _samples["EasingFunctionsScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                      //
      return ::std::make_unique<EasingFunctionsScene>(iCanvas); //
    });                                                         //
  // Extruded Polygon Scene
  _samples["ExtrudedPolygonScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                      //
      return ::std::make_unique<ExtrudedPolygonScene>(iCanvas); //
    });                                                         //
  // Extrusion Scene
  _samples["ExtrusionScene"]
    = ::std::make_tuple(true,
                        [](ICanvas* iCanvas) {                                //
                          return ::std::make_unique<ExtrusionScene>(iCanvas); //
                        });                                                   //
  // Fireworks with Shader Code Scene
  _samples["FireworksWithShaderCodeScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                              //
      return ::std::make_unique<FireworksWithShaderCodeScene>(iCanvas); //
    });                                                                 //
  // Fog Scene
  _samples["FogScene"]
    = ::std::make_tuple(false,
                        [](ICanvas* iCanvas) {                          //
                          return ::std::make_unique<FogScene>(iCanvas); //
                        });                                             //
  // Fresnel Parameters TorusKnot Scene
  _samples["FresnelParametersTorusKnotScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                                 //
      return ::std::make_unique<FresnelParametersTorusKnotScene>(iCanvas); //
    });                                                                    //
  // Height Map Scene
  _samples["HeightMapScene"]
    = ::std::make_tuple(false,
                        [](ICanvas* iCanvas) {                                //
                          return ::std::make_unique<HeightMapScene>(iCanvas); //
                        });                                                   //
  // Hex Icosphere Scene
  _samples["HexIcosphereScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                   //
      return ::std::make_unique<HexIcosphereScene>(iCanvas); //
    });                                                      //
  // House from a Floorplan Scene
  _samples["HouseFromFloorplanScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                         //
      return ::std::make_unique<HouseFromFloorplanScene>(iCanvas); //
    });                                                            //
  // Icosphere Scene
  _samples["IcoSphereScene"]
    = ::std::make_tuple(true,
                        [](ICanvas* iCanvas) {                                //
                          return ::std::make_unique<IcoSphereScene>(iCanvas); //
                        });                                                   //
  // Infinite Loader Scene
  _samples["InfiniteLoaderScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                     //
      return ::std::make_unique<InfiniteLoaderScene>(iCanvas); //
    });                                                        //
  // Lathe Scene
  _samples["LatheScene"]
    = ::std::make_tuple(true,
                        [](ICanvas* iCanvas) {                            //
                          return ::std::make_unique<LatheScene>(iCanvas); //
                        });                                               //
  // Lights Scene
  _samples["LightsScene"]
    = ::std::make_tuple(true,
                        [](ICanvas* iCanvas) {                             //
                          return ::std::make_unique<LightsScene>(iCanvas); //
                        });
  // Lines Mesh Spiral Scene
  _samples["LinesMeshSpiralScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                      //
      return ::std::make_unique<LinesMeshSpiralScene>(iCanvas); //
    });                                                         //
  // Lorenz Attractor Scene
  _samples["LorenzAttractorScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                      //
      return ::std::make_unique<LorenzAttractorScene>(iCanvas); //
    });                                                         //
  // Mandelbrot Fractal Scene
  _samples["MandelbrotFractalScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                        //
      return ::std::make_unique<MandelbrotFractalScene>(iCanvas); //
    });                                                           //
  // Mesh Normals Scene
  _samples["MeshNormalsScene"] = ::std::make_tuple(
    false,
    [](ICanvas* iCanvas) {                                  //
      return ::std::make_unique<MeshNormalsScene>(iCanvas); //
    });                                                     //
  // Merged Meshes Scene
  _samples["MergedMeshesScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                   //
      return ::std::make_unique<MergedMeshesScene>(iCanvas); //
    });                                                      //
  // Perlin Noise Waves Scene
  _samples["PerlinNoiseWavesScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                       //
      return ::std::make_unique<PerlinNoiseWavesScene>(iCanvas); //
    });                                                          //
  // Polygon Mesh Bird Scene
  _samples["PolygonMeshBirdScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                      //
      return ::std::make_unique<PolygonMeshBirdScene>(iCanvas); //
    });                                                         //
  // Polygon Mesh Monkey Scene
  _samples["PolygonMeshMonkeyScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                        //
      return ::std::make_unique<PolygonMeshMonkeyScene>(iCanvas); //
    });                                                           //
  // Polygon Mesh Puzzle Piece Scene
  _samples["PolygonMeshPuzzlePieceScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                             //
      return ::std::make_unique<PolygonMeshPuzzlePieceScene>(iCanvas); //
    });                                                                //
  // Polygon Mesh Stars Scene
  _samples["PolygonMeshStarsScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                       //
      return ::std::make_unique<PolygonMeshStarsScene>(iCanvas); //
    });                                                          //
  // Polygon Mesh Stars With Holes Scene
  _samples["PolygonMeshStarsWithHolesScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                                //
      return ::std::make_unique<PolygonMeshStarsWithHolesScene>(iCanvas); //
    });                                                                   //
  // Polyhedra Scene
  _samples["PolyhedraScene"]
    = ::std::make_tuple(true,
                        [](ICanvas* iCanvas) {                                //
                          return ::std::make_unique<PolyhedraScene>(iCanvas); //
                        });                                                   //
  // Polyhedron Scene
  _samples["PolyhedronScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                 //
      return ::std::make_unique<PolyhedronScene>(iCanvas); //
    });                                                    //
  // Procedural Hex Planet Generation Scene
  _samples["ProceduralHexPlanetGenerationScene"] = ::std::make_tuple(
    false,
    [](ICanvas* iCanvas) {                                                    //
      return ::std::make_unique<ProceduralHexPlanetGenerationScene>(iCanvas); //
    });                                                                       //
  // Pump Jack Scene
  _samples["PumpJackScene"]
    = ::std::make_tuple(false,
                        [](ICanvas* iCanvas) {                               //
                          return ::std::make_unique<PumpJackScene>(iCanvas); //
                        });                                                  //
  // Ribbon Scene
  _samples["RibbonScene"]
    = ::std::make_tuple(true,
                        [](ICanvas* iCanvas) {                             //
                          return ::std::make_unique<RibbonScene>(iCanvas); //
                        });                                                //
  // Rotation and Scaling Scene
  _samples["RotationAndScalingScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                         //
      return ::std::make_unique<RotationAndScalingScene>(iCanvas); //
    });                                                            //
  // Shader Material Box Scene
  _samples["ShaderMaterialBoxScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                        //
      return ::std::make_unique<ShaderMaterialBoxScene>(iCanvas); //
    });                                                           //
  // Shader Material Boxes Scene
  _samples["ShaderMaterialBoxesScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                          //
      return ::std::make_unique<ShaderMaterialBoxesScene>(iCanvas); //
    });                                                             //
  // Shader Material Carved Trees Scene
  _samples["ShaderMaterialCarvedTreesScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                                //
      return ::std::make_unique<ShaderMaterialCarvedTreesScene>(iCanvas); //
    });                                                                   //
  // Shader Material Chains and Gears Scene
  _samples["ShaderMaterialChainsAndGearsScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                                   //
      return ::std::make_unique<ShaderMaterialChainsAndGearsScene>(iCanvas); //
    });                                                                      //
  // Shader Material Cylinders Scene
  _samples["ShaderMaterialCylindersScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                              //
      return ::std::make_unique<ShaderMaterialCylindersScene>(iCanvas); //
    });                                                                 //
  // Shader Material Double Helix Scene
  _samples["ShaderMaterialDoubleHelixScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                                //
      return ::std::make_unique<ShaderMaterialDoubleHelixScene>(iCanvas); //
    });                                                                   //
  // Shader Material Generators Scene
  _samples["ShaderMaterialGeneratorsScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                               //
      return ::std::make_unique<ShaderMaterialGeneratorsScene>(iCanvas); //
    });                                                                  //
  // Shader Material KIFS Scene
  _samples["ShaderMaterialKIFSScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                         //
      return ::std::make_unique<ShaderMaterialKIFSScene>(iCanvas); //
    });                                                            //
  // Shader Material Luminescence Scene
  _samples["ShaderMaterialLuminescenceScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                                 //
      return ::std::make_unique<ShaderMaterialLuminescenceScene>(iCanvas); //
    });                                                                    //
  // Shader Material Menger Sponge Scene
  _samples["ShaderMaterialMengerSpongeScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                                 //
      return ::std::make_unique<ShaderMaterialMengerSpongeScene>(iCanvas); //
    });                                                                    //
  // Shader Material Morning City Scene
  _samples["ShaderMaterialMorningCityScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                                //
      return ::std::make_unique<ShaderMaterialMorningCityScene>(iCanvas); //
    });                                                                   //
  // Shader Material PBR Test Scene
  _samples["ShaderMaterialPBRTestScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                            //
      return ::std::make_unique<ShaderMaterialPBRTestScene>(iCanvas); //
    });                                                               //
  // Shader Material Pegasus Galaxy Scene
  _samples["ShaderMaterialPegasusGalaxyScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                                  //
      return ::std::make_unique<ShaderMaterialPegasusGalaxyScene>(iCanvas); //
    });                                                                     //
  // Shader Material Physically-based SDF Scene
  _samples["ShaderMaterialPhysicallyBasedSDFScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                              //
      return ::std::make_unique<ShaderMaterialPhysicallyBasedSDFScene>( //
        iCanvas);                                                       //
    });                                                                 //
  // Shader Material Planet Shadertoy Scene
  _samples["ShaderMaterialPlanetShadertoyScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                                    //
      return ::std::make_unique<ShaderMaterialPlanetShadertoyScene>(iCanvas); //
    });                                                                       //
  // Shader Material Scene
  _samples["ShaderMaterialScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                     //
      return ::std::make_unique<ShaderMaterialScene>(iCanvas); //
    });                                                        //
  // Shader Material Seascape Scene
  _samples["ShaderMaterialSeascapeScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                             //
      return ::std::make_unique<ShaderMaterialSeascapeScene>(iCanvas); //
    });                                                                //
  // Shader Material Simplicity Scene
  _samples["ShaderMaterialSimplicityScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                               //
      return ::std::make_unique<ShaderMaterialSimplicityScene>(iCanvas); //
    });                                                                  //
  // Shader Material Skybox Clouds Scene
  _samples["ShaderMaterialSkyboxCloudsScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                                 //
      return ::std::make_unique<ShaderMaterialSkyboxCloudsScene>(iCanvas); //
    });                                                                    //
  // Shader Material Sparse Grid Marching Scene
  _samples["ShaderMaterialSparseGridMarchingScene"] = ::std::make_tuple(
    true, [](ICanvas* iCanvas) {
      return ::std::make_unique<ShaderMaterialSparseGridMarchingScene>(iCanvas);
    }); //
  // Shader Material Speed Tracer Scene
  _samples["ShaderMaterialSpeedTracerScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                                //
      return ::std::make_unique<ShaderMaterialSpeedTracerScene>(iCanvas); //
    });                                                                   //
  // Shader Material Skybox Scene
  _samples["ShaderMaterialSkyboxScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                           //
      return ::std::make_unique<ShaderMaterialSkyboxScene>(iCanvas); //
    });                                                              //

  // Shader Material Star Nest Scene
  _samples["ShaderMaterialStarNestScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                             //
      return ::std::make_unique<ShaderMaterialStarNestScene>(iCanvas); //
    });                                                                //
  // Shader Material The Road To Hell Scene
  _samples["ShaderMaterialTheRoadToHellScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                                  //
      return ::std::make_unique<ShaderMaterialTheRoadToHellScene>(iCanvas); //
    });                                                                     //
  // Shader Material The Drive Home Scene
  _samples["ShaderMaterialTheDriveHomeScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                                 //
      return ::std::make_unique<ShaderMaterialTheDriveHomeScene>(iCanvas); //
    });                                                                    //
  // Shader Material Tiles Scene
  _samples["ShaderMaterialTilesScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                          //
      return ::std::make_unique<ShaderMaterialTilesScene>(iCanvas); //
    });                                                             //
  // Shader Material Voronoian Waterway Scene
  _samples["ShaderMaterialVoronoianWaterwayScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) { //
      return ::std::make_unique<ShaderMaterialVoronoianWaterwayScene>(
        iCanvas); //
    });           //
  // Shader Material With Fog Scene
  _samples["ShaderMaterialWithFogScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                            //
      return ::std::make_unique<ShaderMaterialWithFogScene>(iCanvas); //
    });                                                               //
  // Shader Material Wolfenstein 3D Scene
  _samples["ShaderMaterialWolfenstein3DScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                                  //
      return ::std::make_unique<ShaderMaterialWolfenstein3DScene>(iCanvas); //
    });                                                                     //
  // Shader Material Xyptonjtroz Scene
  _samples["ShaderMaterialXyptonjtrozScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                                //
      return ::std::make_unique<ShaderMaterialXyptonjtrozScene>(iCanvas); //
    });                                                                   //
  // Shape Easings Scene
  _samples["ShapeEasingsScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                   //
      return ::std::make_unique<ShapeEasingsScene>(iCanvas); //
    });                                                      //
  // Simple Car Following Path Scene
  _samples["SimpleCarFollowingPathScene"] = ::std::make_tuple(
    false,
    [](ICanvas* iCanvas) {                                             //
      return ::std::make_unique<SimpleCarFollowingPathScene>(iCanvas); //
    });                                                                //
  // Simple Oak Tree Scene
  _samples["SimpleOakTreeScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                    //
      return ::std::make_unique<SimpleOakTreeScene>(iCanvas); //
    });                                                       //
  // Simple Pine Scene
  _samples["SimplePineScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                 //
      return ::std::make_unique<SimplePineScene>(iCanvas); //
    });                                                    //
  // Super Ellipsoid Scene
  _samples["SuperEllipsoidScene"] = ::std::make_tuple(
    true,
    [](ICanvas* iCanvas) {                                     //
      return ::std::make_unique<SuperEllipsoidScene>(iCanvas); //
    });                                                        //
  // Tiled Ground Scene
  _samples["TiledGroundScene"] = ::std::make_tuple(
    false,
    [](ICanvas* iCanvas) {                                  //
      return ::std::make_unique<TiledGroundScene>(iCanvas); //
    });                                                     //
  // Tube Animation Scene
  _samples["TubeAnimationScene"] = ::std::make_tuple(
    false,
    [](ICanvas* iCanvas) {                                    //
      return ::std::make_unique<TubeAnimationScene>(iCanvas); //
    });                                                       //
  // Tube Scene
  _samples["TubeScene"]
    = ::std::make_tuple(true,
                        [](ICanvas* iCanvas) {                           //
                          return ::std::make_unique<TubeScene>(iCanvas); //
                        });                                              //
  // Waves Scene
  _samples["WavesScene"]
    = ::std::make_tuple(true,
                        [](ICanvas* iCanvas) {                            //
                          return ::std::make_unique<WavesScene>(iCanvas); //
                        });                                               //
}

SamplesIndex::~SamplesIndex()
{
}

bool SamplesIndex::isSampleEnabled(const string_t& sampleName)
{
  // Sample exists and is enabled
  return sampleExists(sampleName) && std::get<0>(_samples[sampleName]);
}

bool SamplesIndex::sampleExists(const string_t& sampleName)
{
  return stl_util::contains(_samples, sampleName);
}

std::vector<string_t> SamplesIndex::getSampleNames()
{
  // Extract the enabled sample names from the map
  std::vector<string_t> sampleNames;
  for (auto const& element : _samples) {
    // Check if enabled
    if (std::get<0>(element.second)) {
      sampleNames.emplace_back(element.first);
    }
  }

  // Sort the vector with sample names in ascending order
  std::sort(sampleNames.begin(), sampleNames.end());

  return sampleNames;
}

std::unique_ptr<IRenderableScene>
SamplesIndex::createRenderableScene(const string_t& sampleName,
                                    ICanvas* iCanvas)
{
  if (!isSampleEnabled(sampleName)) {
    return nullptr;
  }

  return std::get<1>(_samples[sampleName])(iCanvas);
}

} // end of namespace Samples
} // end of namespace BABYLON
