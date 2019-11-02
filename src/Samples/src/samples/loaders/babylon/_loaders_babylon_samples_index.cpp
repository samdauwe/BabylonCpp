#include <babylon/samples/loaders/babylon/_loaders_babylon_samples_index.h>

#include <babylon/samples/loaders/babylon/import_babylon_js_logo_scene.h>
#include <babylon/samples/loaders/babylon/import_candle_scene.h>
#include <babylon/samples/loaders/babylon/import_dude_scene.h>
#include <babylon/samples/loaders/babylon/import_dummy3_scene.h>
#include <babylon/samples/loaders/babylon/import_hill_valley_scene.h>
#include <babylon/samples/loaders/babylon/import_meshes_skull_scene.h>
#include <babylon/samples/loaders/babylon/import_rabbit_scene.h>
#include <babylon/samples/loaders/babylon/import_suzanne_scene.h>

namespace BABYLON {
namespace Samples {

_LoadersBabylonSamplesIndex::_LoadersBabylonSamplesIndex()
{
  // -- Loaders examples --

  // Import Babylon JS Logo Scene
  _samples["ImportBabylonJSLogoScene"] =
    [](ICanvas* iCanvas) {
    return std::make_unique<ImportBabylonJSLogoScene>(iCanvas);
  };
  // Import Candle Scene
  _samples["ImportCandleScene"]
    = [](ICanvas* iCanvas) {
    return std::make_unique<ImportCandleScene>(iCanvas);
  };
  // Import Dude Scene
  _samples["ImportDudeScene"]
    = [](ICanvas* iCanvas) {
    return std::make_unique<ImportDudeScene>(iCanvas);
  };
  // Import Dummy 3 Scene
  _samples["ImportDummy3Scene"]
    = [](ICanvas* iCanvas) {
    return std::make_unique<ImportDummy3Scene>(iCanvas);
  };
  // Import Hill Valley Scene
  _samples["ImportHillValleyScene"]
    = [](ICanvas* iCanvas) {
        return MakeImportHillValleyScene(iCanvas);
  };
  // Import Meshes Skull Scene
  _samples["ImportMeshesSkullScene"] =     
    [](ICanvas* iCanvas) {
      return std::make_unique<ImportMeshesSkullScene>(iCanvas);
    };
  // Import Rabbit Scene
  _samples["ImportRabbitScene"]
    = [](ICanvas* iCanvas) {
    return std::make_unique<ImportRabbitScene>(iCanvas);
  };

  // Import Suzanne Scene
  _samples["ImportSuzanneScene"]
    = [](ICanvas* iCanvas) {
    return std::make_unique<ImportSuzanneScene>(iCanvas);
  };
}

} // end of namespace Samples
} // end of namespace BABYLON
