#include <babylon/samples/loaders/_loaders_samples_index.h>

#include <babylon/samples/loaders/import_meshes_skull_scene.h>

namespace BABYLON {
namespace Samples {

_LoadersSamplesIndex::_LoadersSamplesIndex()
{
  // -- Loaders examples --

  // Import Meshes Skull Scene
  _samples["ImportMeshesSkullScene"] = std::make_tuple(
    true,                                                         //
    [](ICanvas* iCanvas) {                                        //
      return std::make_unique<ImportMeshesSkullScene>(iCanvas); //
    });                                                           //
}

_LoadersSamplesIndex::~_LoadersSamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
