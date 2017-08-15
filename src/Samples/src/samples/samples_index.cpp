#include <babylon/samples/samples_index.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/samples/animations/animations_scene.h>
#include <babylon/samples/animations/easing/easing_functions_scene.h>
#include <babylon/samples/animations/pump_jack_scene.h>
#include <babylon/samples/animations/tube_animation_scene.h>
#include <babylon/samples/effects/fog_scene.h>
#include <babylon/samples/extensions/noisegeneration/perlin_noise_waves_scene.h>
#include <babylon/samples/extensions/polyhedron/polyhedra_scene.h>
#include <babylon/samples/generic/infinite_loader_scene.h>
#include <babylon/samples/generic/lorenz_attractor_scene.h>
#include <babylon/samples/generic/mandelbrot_fractal_scene.h>
#include <babylon/samples/generic/super_ellipsoid_scene.h>
#include <babylon/samples/generic/waves_scene.h>
#include <babylon/samples/lights/lights_scene.h>
#include <babylon/samples/materials/shader_material_box_scene.h>
#include <babylon/samples/materials/shader_material_boxes_scene.h>
#include <babylon/samples/materials/shader_material_cylinders_scene.h>
#include <babylon/samples/materials/shader_material_scene.h>
#include <babylon/samples/materials/shader_material_skybox_scene.h>
#include <babylon/samples/mesh/basic_elements_scene.h>
#include <babylon/samples/mesh/basic_scene.h>
#include <babylon/samples/mesh/cuboid_scene.h>
#include <babylon/samples/mesh/extrusion_scene.h>
#include <babylon/samples/mesh/ico_sphere_scene.h>
#include <babylon/samples/mesh/lathe_scene.h>
#include <babylon/samples/mesh/mesh_normals_scene.h>
#include <babylon/samples/mesh/polygonmesh/polygon_mesh_bird_scene.h>
#include <babylon/samples/mesh/polygonmesh/polygon_mesh_monkey_scene.h>
#include <babylon/samples/mesh/polygonmesh/polygon_mesh_puzzle_piece_scene.h>
#include <babylon/samples/mesh/polygonmesh/polygon_mesh_stars_scene.h>
#include <babylon/samples/mesh/polygonmesh/polygon_mesh_stars_with_holes_scene.h>
#include <babylon/samples/mesh/polyhedron_scene.h>
#include <babylon/samples/mesh/rotation_and_scaling_scene.h>
#include <babylon/samples/mesh/tiled_ground_scene.h>
#include <babylon/samples/mesh/tube_scene.h>

namespace BABYLON {
namespace Samples {

std::unordered_map<std::string, Sample> SamplesIndex::Samples
  = ([]() -> std::unordered_map<std::string, Sample> {
      std::unordered_map<std::string, Sample> samples;
      // Animations Scene
      samples["AnimationsScene"]
        = {true, [](ICanvas* iCanvas) {                         //
             return std::make_unique<AnimationsScene>(iCanvas); //
           }};                                                  //
      // Basic Elements Scene
      samples["BasicElementsScene"]
        = {true, [](ICanvas* iCanvas) {                            //
             return std::make_unique<BasicElementsScene>(iCanvas); //
           }};                                                     //
      // Basic Scene
      samples["BasicScene"]
        = {true, [](ICanvas* iCanvas) {                    //
             return std::make_unique<BasicScene>(iCanvas); //
           }};                                             //
      // Cuboid Scene
      samples["CuboidScene"]
        = {true,
           [](ICanvas* iCanvas) {                           //
             return std::make_unique<CuboidScene>(iCanvas); //
           }};                                              //
      // Easing Functions Scene
      samples["EasingFunctionsScene"]
        = {false,
           [](ICanvas* iCanvas) {                                    //
             return std::make_unique<EasingFunctionsScene>(iCanvas); //
           }};                                                       //
      // Extrusion Scene
      samples["ExtrusionScene"]
        = {true,
           [](ICanvas* iCanvas) {                              //
             return std::make_unique<ExtrusionScene>(iCanvas); //
           }};                                                 //
      // Fog Scene
      samples["FogScene"] = {false,
                             [](ICanvas* iCanvas) {                        //
                               return std::make_unique<FogScene>(iCanvas); //
                             }};                                           //
      // Icosphere Scene
      samples["IcoSphereScene"]
        = {true,
           [](ICanvas* iCanvas) {                              //
             return std::make_unique<IcoSphereScene>(iCanvas); //
           }};                                                 //
      // Infinite Loader Scene
      samples["InfiniteLoaderScene"]
        = {true,
           [](ICanvas* iCanvas) {                                   //
             return std::make_unique<InfiniteLoaderScene>(iCanvas); //
           }};                                                      //
      // Lathe Scene
      samples["LatheScene"]
        = {true,
           [](ICanvas* iCanvas) {                          //
             return std::make_unique<LatheScene>(iCanvas); //
           }};                                             //
      // Lights Scene
      samples["LightsScene"]
        = {true,
           [](ICanvas* iCanvas) {                           //
             return std::make_unique<LightsScene>(iCanvas); //
           }};                                              //
      // Lorenz Attractor Scene
      samples["LorenzAttractorScene"]
        = {true,
           [](ICanvas* iCanvas) {                                    //
             return std::make_unique<LorenzAttractorScene>(iCanvas); //
           }};                                                       //
      // Mandelbrot Fractal Scene
      samples["MandelbrotFractalScene"]
        = {true,
           [](ICanvas* iCanvas) {                                      //
             return std::make_unique<MandelbrotFractalScene>(iCanvas); //
           }};                                                         //
      // Mesh Normals Scene
      samples["MeshNormalsScene"]
        = {false,
           [](ICanvas* iCanvas) {                                //
             return std::make_unique<MeshNormalsScene>(iCanvas); //
           }};                                                   //
      // Perlin Noise Waves Scene
      samples["PerlinNoiseWavesScene"]
        = {true,
           [](ICanvas* iCanvas) {                                     //
             return std::make_unique<PerlinNoiseWavesScene>(iCanvas); //
           }};                                                        //
      // Polygon Mesh Bird Scene
      samples["PolygonMeshBirdScene"]
        = {true,
           [](ICanvas* iCanvas) {                                    //
             return std::make_unique<PolygonMeshBirdScene>(iCanvas); //
           }};                                                       //
      // Polygon Mesh Monkey Scene
      samples["PolygonMeshMonkeyScene"]
        = {true,
           [](ICanvas* iCanvas) {                                      //
             return std::make_unique<PolygonMeshMonkeyScene>(iCanvas); //
           }};                                                         //
      // Polygon Mesh Puzzle Piece Scene
      samples["PolygonMeshPuzzlePieceScene"]
        = {true,
           [](ICanvas* iCanvas) {                                           //
             return std::make_unique<PolygonMeshPuzzlePieceScene>(iCanvas); //
           }};                                                              //
      // Polygon Mesh Stars Scene
      samples["PolygonMeshStarsScene"]
        = {true,
           [](ICanvas* iCanvas) {                                     //
             return std::make_unique<PolygonMeshStarsScene>(iCanvas); //
           }};                                                        //
      // Polygon Mesh Stars With Holes Scene
      samples["PolygonMeshStarsWithHolesScene"] = {
        true,
        [](ICanvas* iCanvas) {                                              //
          return std::make_unique<PolygonMeshStarsWithHolesScene>(iCanvas); //
        }};                                                                 //
      // Polyhedra Scene
      samples["PolyhedraScene"]
        = {true,
           [](ICanvas* iCanvas) {                              //
             return std::make_unique<PolyhedraScene>(iCanvas); //
           }};                                                 //
      // Polyhedron Scene
      samples["PolyhedronScene"]
        = {true,
           [](ICanvas* iCanvas) {                               //
             return std::make_unique<PolyhedronScene>(iCanvas); //
           }};                                                  //
      // Pump Jack Scene
      samples["PumpJackScene"]
        = {false,
           [](ICanvas* iCanvas) {                             //
             return std::make_unique<PumpJackScene>(iCanvas); //
           }};                                                //
      // Rotation and Scaling Scene
      samples["RotationAndScalingScene"]
        = {true,
           [](ICanvas* iCanvas) {                                       //
             return std::make_unique<RotationAndScalingScene>(iCanvas); //
           }};                                                          //
      // Shader Material Box Scene
      samples["ShaderMaterialBoxScene"]
        = {true,
           [](ICanvas* iCanvas) {                                      //
             return std::make_unique<ShaderMaterialBoxScene>(iCanvas); //
           }};                                                         //
      // Shader Material Boxes Scene
      samples["ShaderMaterialBoxesScene"]
        = {true,
           [](ICanvas* iCanvas) {                                        //
             return std::make_unique<ShaderMaterialBoxesScene>(iCanvas); //
           }};                                                           //
      // Shader Material Cylinders Scene
      samples["ShaderMaterialCylindersScene"]
        = {true,
           [](ICanvas* iCanvas) {                                            //
             return std::make_unique<ShaderMaterialCylindersScene>(iCanvas); //
           }};                                                               //
      // Shader Material Scene
      samples["ShaderMaterialScene"]
        = {true,
           [](ICanvas* iCanvas) {                                   //
             return std::make_unique<ShaderMaterialScene>(iCanvas); //
           }};                                                      //
      // Shader Material Skybox Scene
      samples["ShaderMaterialSkyboxScene"]
        = {true,
           [](ICanvas* iCanvas) {                                         //
             return std::make_unique<ShaderMaterialSkyboxScene>(iCanvas); //
           }};                                                            //
      // Super Ellipsoid Scene
      samples["SuperEllipsoidScene"]
        = {true,
           [](ICanvas* iCanvas) {                                   //
             return std::make_unique<SuperEllipsoidScene>(iCanvas); //
           }};                                                      //
      // Tiled Ground Scene
      samples["TiledGroundScene"]
        = {false,
           [](ICanvas* iCanvas) {                                //
             return std::make_unique<TiledGroundScene>(iCanvas); //
           }};                                                   //
      // Tube Animation Scene
      samples["TubeAnimationScene"]
        = {false,
           [](ICanvas* iCanvas) {                                  //
             return std::make_unique<TubeAnimationScene>(iCanvas); //
           }};                                                     //
      // Tube Scene
      samples["TubeScene"] = {true,
                              [](ICanvas* iCanvas) {                         //
                                return std::make_unique<TubeScene>(iCanvas); //
                              }};                                            //
      // Waves Scene
      samples["WavesScene"]
        = {true,
           [](ICanvas* iCanvas) {                          //
             return std::make_unique<WavesScene>(iCanvas); //
           }};                                             //
      return samples;
    })();

bool SamplesIndex::isSampleEnabled(const std::string& sampleName)
{
  return sampleExists(sampleName) && Samples[sampleName].enabled;
}

bool SamplesIndex::sampleExists(const std::string& sampleName)
{
  return stl_util::contains(Samples, sampleName);
}

std::vector<std::string> SamplesIndex::getSampleNames()
{
  // Extract the enabled sample names from the map
  std::vector<std::string> sampleNames;
  for (auto const& element : Samples) {
    if (element.second.enabled) {
      sampleNames.emplace_back(element.first);
    }
  }

  // Sort the vector with sample names in ascending order
  std::sort(sampleNames.begin(), sampleNames.end());

  return sampleNames;
}

std::unique_ptr<IRenderableScene>
SamplesIndex::createRenderableScene(const std::string& sampleName,
                                    ICanvas* iCanvas)
{
  if (!isSampleEnabled(sampleName)) {
    return nullptr;
  }

  return Samples[sampleName].factoryMethod(iCanvas);
}

} // end of namespace Samples
} // end of namespace BABYLON
