#include <babylon/samples/loaders/_loaders_samples_index.h>

#include <babylon/samples/loaders/import_babylon_js_logo_scene.h>
#include <babylon/samples/loaders/import_candle_scene.h>
#include <babylon/samples/loaders/import_dude_scene.h>
#include <babylon/samples/loaders/import_dummy3_scene.h>
#include <babylon/samples/loaders/import_meshes_skull_scene.h>
#include <babylon/samples/loaders/import_rabbit_scene.h>
#include <babylon/samples/loaders/import_suzanne_scene.h>

// Loaders plugins
#ifdef WITH_LOADERS
#include <babylon/samples/loaders/gltf/import_2cylinder_engine_scene.h>
#include <babylon/samples/loaders/gltf/import_animated_triangle_scene.h>
#include <babylon/samples/loaders/gltf/import_box_interleaved_scene.h>
#include <babylon/samples/loaders/gltf/import_box_scene.h>
#include <babylon/samples/loaders/gltf/import_box_textured_non_power_of_two_scene.h>
#include <babylon/samples/loaders/gltf/import_box_textured_scene.h>
#include <babylon/samples/loaders/gltf/import_box_vertex_colors_scene.h>
#include <babylon/samples/loaders/gltf/import_cameras_scene.h>
#include <babylon/samples/loaders/gltf/import_duck_scene.h>
#include <babylon/samples/loaders/gltf/import_interpolation_test_scene.h>
#include <babylon/samples/loaders/gltf/import_reciprocating_saw_scene.h>
#include <babylon/samples/loaders/gltf/import_simple_meshes_scene.h>
#include <babylon/samples/loaders/gltf/import_simple_sparse_accessor_scene.h>
#include <babylon/samples/loaders/gltf/import_triangle_scene.h>
#include <babylon/samples/loaders/gltf/import_triangle_without_indices_scene.h>
#endif

namespace BABYLON {
namespace Samples {

_LoadersSamplesIndex::_LoadersSamplesIndex()
{
  // -- Loaders examples --

  // Import Babylon JS Logo Scene
  _samples["ImportBabylonJSLogoScene"] = std::make_tuple(
    true,                                                         //
    [](ICanvas* iCanvas) {                                        //
      return std::make_unique<ImportBabylonJSLogoScene>(iCanvas); //
    });                                                           //
  // Import Candle Scene
  _samples["ImportCandleScene"]
    = std::make_tuple(true,                                                  //
                      [](ICanvas* iCanvas) {                                 //
                        return std::make_unique<ImportCandleScene>(iCanvas); //
                      });                                                    //
  // Import Dude Scene
  _samples["ImportDudeScene"]
    = std::make_tuple(true,                                                //
                      [](ICanvas* iCanvas) {                               //
                        return std::make_unique<ImportDudeScene>(iCanvas); //
                      });                                                  //
  // Import Dummy 3 Scene
  _samples["ImportDummy3Scene"]
    = std::make_tuple(true,                                                  //
                      [](ICanvas* iCanvas) {                                 //
                        return std::make_unique<ImportDummy3Scene>(iCanvas); //
                      });                                                    //
  // Import Meshes Skull Scene
  _samples["ImportMeshesSkullScene"] = std::make_tuple(
    true,                                                       //
    [](ICanvas* iCanvas) {                                      //
      return std::make_unique<ImportMeshesSkullScene>(iCanvas); //
    });                                                         //
  // Import Rabbit Scene
  _samples["ImportRabbitScene"]
    = std::make_tuple(true,                                                  //
                      [](ICanvas* iCanvas) {                                 //
                        return std::make_unique<ImportRabbitScene>(iCanvas); //
                      });                                                    //
  // Import Suzanne Scene
  _samples["ImportSuzanneScene"]
    = std::make_tuple(true,                                                   //
                      [](ICanvas* iCanvas) {                                  //
                        return std::make_unique<ImportSuzanneScene>(iCanvas); //
                      });                                                     //
#ifdef WITH_LOADERS
  // Import 2 Cylinder Engine Scene (glTF)
  _samples["Import2CylinderEngineScene"] = std::make_tuple(
    false,                                                          //
    [](ICanvas* iCanvas) {                                          //
      return std::make_unique<Import2CylinderEngineScene>(iCanvas); //
    });                                                             //
  // Import Animated Triangle Scene (glTF)
  _samples["ImportAnimatedTriangleScene"] = std::make_tuple(
    true,                                                            //
    [](ICanvas* iCanvas) {                                           //
      return std::make_unique<ImportAnimatedTriangleScene>(iCanvas); //
    });                                                              //
  // Import Box Interleaved Scene (glTF)
  _samples["ImportBoxInterleavedScene"] = std::make_tuple(
    true,                                                          //
    [](ICanvas* iCanvas) {                                         //
      return std::make_unique<ImportBoxInterleavedScene>(iCanvas); //
    });                                                            //
  // Import Box Scene (glTF)
  _samples["ImportBoxScene"]
    = std::make_tuple(true,                                               //
                      [](ICanvas* iCanvas) {                              //
                        return std::make_unique<ImportBoxScene>(iCanvas); //
                      });                                                 //
  // Import Box Textured Non Power Of Two Scene (glTF)
  _samples["ImportBoxTexturedNonPowerOfTwoScene"] = std::make_tuple(
    false,                                                                   //
    [](ICanvas* iCanvas) {                                                   //
      return std::make_unique<ImportBoxTexturedNonPowerOfTwoScene>(iCanvas); //
    });                                                                      //
  // Import Box Textured Scene (glTF)
  _samples["ImportBoxTexturedScene"] = std::make_tuple(
    true,                                                       //
    [](ICanvas* iCanvas) {                                      //
      return std::make_unique<ImportBoxTexturedScene>(iCanvas); //
    });                                                         //
  // Import Box Vertex Colors Scene (glTF)
  _samples["ImportBoxVertexColorsScene"] = std::make_tuple(
    true,                                                           //
    [](ICanvas* iCanvas) {                                          //
      return std::make_unique<ImportBoxVertexColorsScene>(iCanvas); //
    });                                                             //
  // Import Cameras Scene (glTF)
  _samples["ImportCamerasScene"]
    = std::make_tuple(true,                                                   //
                      [](ICanvas* iCanvas) {                                  //
                        return std::make_unique<ImportCamerasScene>(iCanvas); //
                      });                                                     //
  // Import Duck Scene (glTF)
  _samples["ImportDuckScene"]
    = std::make_tuple(false,                                               //
                      [](ICanvas* iCanvas) {                               //
                        return std::make_unique<ImportDuckScene>(iCanvas); //
                      });                                                  //
  // Import Interpolation Test Scene (glTF)
  _samples["ImportInterpolationTestScene"] = std::make_tuple(
    false,                                                            //
    [](ICanvas* iCanvas) {                                            //
      return std::make_unique<ImportInterpolationTestScene>(iCanvas); //
    });                                                               //
  // Import Reciprocating Saw Scene (glTF)
  _samples["ImportReciprocatingSawScene"] = std::make_tuple(
    false,                                                           //
    [](ICanvas* iCanvas) {                                           //
      return std::make_unique<ImportReciprocatingSawScene>(iCanvas); //
    });                                                              //
  // Import Simple Meshes Scene (glTF)
  _samples["ImportSimpleMeshesScene"] = std::make_tuple(
    true,                                                        //
    [](ICanvas* iCanvas) {                                       //
      return std::make_unique<ImportSimpleMeshesScene>(iCanvas); //
    });                                                          //
  // Import Simple Sparse Accessor Scene (glTF)
  _samples["ImportSimpleSparseAccessorScene"] = std::make_tuple(
    true,                                                                //
    [](ICanvas* iCanvas) {                                               //
      return std::make_unique<ImportSimpleSparseAccessorScene>(iCanvas); //
    });                                                                  //
  // Import Triangle Scene (glTF)
  _samples["ImportTriangleScene"] = std::make_tuple(
    true,                                                    //
    [](ICanvas* iCanvas) {                                   //
      return std::make_unique<ImportTriangleScene>(iCanvas); //
    });                                                      //
  // Import Triangle Without Indices Scene (glTF)
  _samples["ImportTriangleWithoutIndicesScene"] = std::make_tuple(
    true,                                                                  //
    [](ICanvas* iCanvas) {                                                 //
      return std::make_unique<ImportTriangleWithoutIndicesScene>(iCanvas); //
    });                                                                    //
#endif
}

_LoadersSamplesIndex::~_LoadersSamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
