#include <babylon/samples/proceduraltextureslibrary/_procedural_textures_library_samples_index.h>

#include <babylon/samples/proceduraltextureslibrary/brick_procedural_texture_scene.h>

namespace BABYLON {
namespace Samples {

_ProceduralTexturesLibrarySamplesIndex::_ProceduralTexturesLibrarySamplesIndex()
{
  // -- Procedural Textures Library examples --

  // Brick Procedural Texture Scene
  _samples["BrickProceduralTextureScene"] = std::make_tuple(
    true,                                                            //
    [](ICanvas* iCanvas) {                                           //
      return std::make_unique<BrickProceduralTextureScene>(iCanvas); //
    });                                                              //
}

_ProceduralTexturesLibrarySamplesIndex::
  ~_ProceduralTexturesLibrarySamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
