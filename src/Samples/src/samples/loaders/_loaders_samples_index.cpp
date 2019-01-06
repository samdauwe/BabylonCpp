#include <babylon/samples/loaders/_loaders_samples_index.h>

#include <babylon/samples/loaders/import_babylon_js_logo_scene.h>
#include <babylon/samples/loaders/import_candle_scene.h>
#include <babylon/samples/loaders/import_meshes_skull_scene.h>
#include <babylon/samples/loaders/import_rabbit_scene.h>

namespace BABYLON {
namespace Samples {

_LoadersSamplesIndex::_LoadersSamplesIndex()
{
  // -- Loaders examples --

  // Import Babylon JS Logo Scene
  _samples["ImportBabylonJSLogoScene"] = std::make_tuple(
    true,                                                        //
    [](ICanvas* iCanvas) {                                        //
      return std::make_unique<ImportBabylonJSLogoScene>(iCanvas); //
    });                                                           //
  // Import Candle Scene
  _samples["ImportCandleScene"]
    = std::make_tuple(true,                                                  //
                      [](ICanvas* iCanvas) {                                 //
                        return std::make_unique<ImportCandleScene>(iCanvas); //
                      });                                                    //
  // Import Meshes Skull Scene
  _samples["ImportMeshesSkullScene"] = std::make_tuple(
    true,                                                       //
    [](ICanvas* iCanvas) {                                      //
      return std::make_unique<ImportMeshesSkullScene>(iCanvas); //
    });                                                         //
  // Import Rabbit Scene
  _samples["ImportRabbitScene"]
    = std::make_tuple(true,                                                 //
                      [](ICanvas* iCanvas) {                                 //
                        return std::make_unique<ImportRabbitScene>(iCanvas); //
                      });                                                    //
}

_LoadersSamplesIndex::~_LoadersSamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
