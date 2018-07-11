#include <babylon/samples/textures/_textures_samples_index.h>

#include <babylon/samples/textures/bump_texture_scene.h>

namespace BABYLON {
namespace Samples {

_TexturesSamplesIndex::_TexturesSamplesIndex()
{
  // -- Textures examples --

  // Bump Texture Scene
  _samples["BumpTextureScene"] = ::std::make_tuple(
    true,                                                   //
    [](ICanvas* iCanvas) {                                  //
      return ::std::make_unique<BumpTextureScene>(iCanvas); //
    });                                                     //
}

_TexturesSamplesIndex::~_TexturesSamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
