#include <babylon/samples/loaders/gltf/_loaders_gtlf_samples_index.h>

// Loaders plugins
#ifdef WITH_LOADERS
// -- Simple models for testing individual features --
#include <babylon/samples/loaders/gltf/simplemodels/animated_morph_cube_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/animated_morph_sphere_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/animated_triangle_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/cameras_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/interpolation_test_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/simple_material_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/simple_meshes_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/simple_morph_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/simple_skin_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/simple_sparse_accessor_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/simple_texture_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/triangle_scene.h>
#include <babylon/samples/loaders/gltf/simplemodels/triangle_without_indices_scene.h>

// -- More complex models --
#include <babylon/samples/loaders/gltf/morecomplexmodels/2cylinder_engine_scene.h>
#include <babylon/samples/loaders/gltf/morecomplexmodels/box_interleaved_scene.h>
#include <babylon/samples/loaders/gltf/morecomplexmodels/box_scene.h>
#include <babylon/samples/loaders/gltf/morecomplexmodels/box_textured_non_power_of_two_scene.h>
#include <babylon/samples/loaders/gltf/morecomplexmodels/box_textured_scene.h>
#include <babylon/samples/loaders/gltf/morecomplexmodels/box_vertex_colors_scene.h>
#include <babylon/samples/loaders/gltf/morecomplexmodels/duck_scene.h>
#include <babylon/samples/loaders/gltf/morecomplexmodels/reciprocating_saw_scene.h>

// -- PBR models --
#include <babylon/samples/loaders/gltf/pbrmodels/avocado_scene.h>

// -- Further PBR models --
#include <babylon/samples/loaders/gltf/furtherpbrmodels/animated_cube_scene.h>
#include <babylon/samples/loaders/gltf/furtherpbrmodels/antique_camera_scene.h>
#include <babylon/samples/loaders/gltf/furtherpbrmodels/cube_scene.h>
#include <babylon/samples/loaders/gltf/furtherpbrmodels/sci_fi_helmet_scene.h>
#include <babylon/samples/loaders/gltf/furtherpbrmodels/suzanne_scene.h>
#include <babylon/samples/loaders/gltf/furtherpbrmodels/two_sided_plane_scene.h>

#endif

namespace BABYLON {
namespace Samples {

_LoadersGLTFSamplesIndex::_LoadersGLTFSamplesIndex()
{

#ifdef WITH_LOADERS
  // -- Simple models for testing individual features --

  // Animated Morph Cube Scene (glTF)
  _samples["AnimatedMorphCubeGLTFScene"] = std::make_tuple(
    true,                                                       //
    [](ICanvas* iCanvas) {                                      //
      return std::make_unique<AnimatedMorphCubeScene>(iCanvas); //
    });                                                         //
  // Animated Morph Sphere Scene (glTF)
  _samples["AnimatedMorphSphereGLTFScene"] = std::make_tuple(
    true,                                                         //
    [](ICanvas* iCanvas) {                                        //
      return std::make_unique<AnimatedMorphSphereScene>(iCanvas); //
    });                                                           //
  // Animated Triangle Scene (glTF)
  _samples["AnimatedTriangleGLTFScene"] = std::make_tuple(
    true,                                                      //
    [](ICanvas* iCanvas) {                                     //
      return std::make_unique<AnimatedTriangleScene>(iCanvas); //
    });                                                        //
  // Cameras Scene (glTF)
  _samples["CamerasGLTFScene"]
    = std::make_tuple(true,                                                 //
                      [](ICanvas* iCanvas) {                                //
                        return std::make_unique<CamerasGLTFScene>(iCanvas); //
                      });                                                   //
  // Interpolation Test Scene (glTF)
  _samples["InterpolationTestGLTFScene"] = std::make_tuple(
    false,                                                      //
    [](ICanvas* iCanvas) {                                      //
      return std::make_unique<InterpolationTestScene>(iCanvas); //
    });                                                         //
  // Simple Material Scene (glTF)
  _samples["SimpleMaterialGLTFScene"] = std::make_tuple(
    true,                                                        //
    [](ICanvas* iCanvas) {                                       //
      return std::make_unique<SimpleMaterialGLTFScene>(iCanvas); //
    });                                                          //
  // Simple Meshes Scene (glTF)
  _samples["SimpleMeshesGLTFScene"]
    = std::make_tuple(true,                                                  //
                      [](ICanvas* iCanvas) {                                 //
                        return std::make_unique<SimpleMeshesScene>(iCanvas); //
                      });                                                    //
  // Simple Morph Scene (glTF)
  _samples["SimpleMorphGLTFScene"]
    = std::make_tuple(true,                                                 //
                      [](ICanvas* iCanvas) {                                //
                        return std::make_unique<SimpleMorphScene>(iCanvas); //
                      });                                                   //
  // Simple Skin Scene (glTF)
  _samples["SimpleSkinGLTFScene"]
    = std::make_tuple(true,                                                //
                      [](ICanvas* iCanvas) {                               //
                        return std::make_unique<SimpleSkinScene>(iCanvas); //
                      });                                                  //
  // Simple Sparse Accessor Scene (glTF)
  _samples["SimpleSparseAccessorGLTFScene"] = std::make_tuple(
    true,                                                          //
    [](ICanvas* iCanvas) {                                         //
      return std::make_unique<SimpleSparseAccessorScene>(iCanvas); //
    });                                                            //
  // Simple Texture Scene (glTF)
  _samples["SimpleTextureGLTFScene"]
    = std::make_tuple(true,                                                   //
                      [](ICanvas* iCanvas) {                                  //
                        return std::make_unique<SimpleTextureScene>(iCanvas); //
                      });                                                     //
  // Triangle Scene (glTF)
  _samples["TriangleGLTFScene"]
    = std::make_tuple(true,                                              //
                      [](ICanvas* iCanvas) {                             //
                        return std::make_unique<TriangleScene>(iCanvas); //
                      });                                                //
  // Triangle Without Indices Scene (glTF)
  _samples["TriangleWithoutIndicesGLTFScene"] = std::make_tuple(
    true,                                                            //
    [](ICanvas* iCanvas) {                                           //
      return std::make_unique<TriangleWithoutIndicesScene>(iCanvas); //
    });                                                              //

  // -- More complex models --

  // 2 Cylinder Engine Scene (glTF)
  _samples["2CylinderEngineGLTFScene"] = std::make_tuple(
    false,                                                      //
    [](ICanvas* iCanvas) {                                      //
      return std::make_unique<TwoCylinderEngineScene>(iCanvas); //
    });                                                         //
  // Box Interleaved Scene (glTF)
  _samples["BoxInterleavedGLTFScene"] = std::make_tuple(
    true,                                                    //
    [](ICanvas* iCanvas) {                                   //
      return std::make_unique<BoxInterleavedScene>(iCanvas); //
    });                                                      //
  // Box Scene (glTF)
  _samples["BoxGLTFScene"]
    = std::make_tuple(true,                                         //
                      [](ICanvas* iCanvas) {                        //
                        return std::make_unique<BoxScene>(iCanvas); //
                      });                                           //
  // Box Textured Non Power Of Two Scene (glTF)
  _samples["BoxTexturedNonPowerOfTwoGLTFScene"] = std::make_tuple(
    false,                                                             //
    [](ICanvas* iCanvas) {                                             //
      return std::make_unique<BoxTexturedNonPowerOfTwoScene>(iCanvas); //
    });                                                                //
  // Box Textured Scene (glTF)
  _samples["BoxTexturedGLTFScene"]
    = std::make_tuple(true,                                                 //
                      [](ICanvas* iCanvas) {                                //
                        return std::make_unique<BoxTexturedScene>(iCanvas); //
                      });                                                   //
  // Box Vertex Colors Scene (glTF)
  _samples["BoxVertexColorsGLTFScene"] = std::make_tuple(
    true,                                                     //
    [](ICanvas* iCanvas) {                                    //
      return std::make_unique<BoxVertexColorsScene>(iCanvas); //
    });                                                       //
  // Duck Scene (glTF)
  _samples["DuckGLTFScene"]
    = std::make_tuple(true,                                          //
                      [](ICanvas* iCanvas) {                         //
                        return std::make_unique<DuckScene>(iCanvas); //
                      });                                            //
  // Reciprocating Saw Scene (glTF)
  _samples["ReciprocatingSawGLTFScene"] = std::make_tuple(
    false,                                                     //
    [](ICanvas* iCanvas) {                                     //
      return std::make_unique<ReciprocatingSawScene>(iCanvas); //
    });                                                        //

  // -- PBR models --

  // Avocado Scene (glTF)
  _samples["AvocadoGLTFScene"]
    = std::make_tuple(true,                                             //
                      [](ICanvas* iCanvas) {                            //
                        return std::make_unique<AvocadoScene>(iCanvas); //
                      });                                               //

  // -- Further PBR models --

  // Animated Cube Scene (glTF)
  _samples["AnimatedCubeGLTFScene"]
    = std::make_tuple(true,                                                  //
                      [](ICanvas* iCanvas) {                                 //
                        return std::make_unique<AnimatedCubeScene>(iCanvas); //
                      });                                                    //
  // Antique Camera Scene (glTF)
  _samples["AntiqueCameraGLTFScene"]
    = std::make_tuple(true,                                                   //
                      [](ICanvas* iCanvas) {                                  //
                        return std::make_unique<AntiqueCameraScene>(iCanvas); //
                      });                                                     //
  // Cube Scene (glTF)
  _samples["CubeGLTFScene"]
    = std::make_tuple(true,                                          //
                      [](ICanvas* iCanvas) {                         //
                        return std::make_unique<CubeScene>(iCanvas); //
                      });                                            //
  // Sci Fi Helmet Scene (glTF)
  _samples["SciFiHelmetGLTFScene"]
    = std::make_tuple(true,                                                 //
                      [](ICanvas* iCanvas) {                                //
                        return std::make_unique<SciFiHelmetScene>(iCanvas); //
                      });                                                   //
  // Suzanne Scene (glTF)
  _samples["SuzanneGLTFScene"]
    = std::make_tuple(true,                                             //
                      [](ICanvas* iCanvas) {                            //
                        return std::make_unique<SuzanneScene>(iCanvas); //
                      });                                               //
  // Two Sided Plane Scene (glTF)
  _samples["TwoSidedPlaneGLTFScene"]
    = std::make_tuple(true,                                                   //
                      [](ICanvas* iCanvas) {                                  //
                        return std::make_unique<TwoSidedPlaneScene>(iCanvas); //
                      });                                                     //
#endif
}

_LoadersGLTFSamplesIndex::~_LoadersGLTFSamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
