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
#include <babylon/samples/loaders/gltf/import_box_scene.h>
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
  // Import Box Scene (glTF)
  _samples["ImportBoxScene"]
    = std::make_tuple(false,                                              //
                      [](ICanvas* iCanvas) {                              //
                        return std::make_unique<ImportBoxScene>(iCanvas); //
                      });                                                 //
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
