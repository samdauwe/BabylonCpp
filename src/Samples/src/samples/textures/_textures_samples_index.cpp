#include <babylon/samples/textures/_textures_samples_index.h>

#include <babylon/samples/textures/bump_texture_scene.h>
#include <babylon/samples/textures/bump_texture_spheres_scene.h>
#include <babylon/samples/textures/procedural_textures_scene.h>

namespace BABYLON {
namespace Samples {

_TexturesSamplesIndex::_TexturesSamplesIndex()
{
  // -- Textures examples --

  // Bump Texture Scene
  _samples["BumpTextureScene"]
    = std::make_tuple(true,                                                 //
                      [](ICanvas* iCanvas) {                                //
                        return std::make_unique<BumpTextureScene>(iCanvas); //
                      });                                                   //
  // Bump Texture Spheres Scene
  _samples["BumpTextureSpheresScene"] = std::make_tuple(
    true,                                                        //
    [](ICanvas* iCanvas) {                                       //
      return std::make_unique<BumpTextureSpheresScene>(iCanvas); //
    });                                                          //
  // Procedural Textures Scene
  _samples["ProceduralTexturesScene"] = std::make_tuple(
    false,                                                       //
    [](ICanvas* iCanvas) {                                       //
      return std::make_unique<ProceduralTexturesScene>(iCanvas); //
    });                                                          //
}

_TexturesSamplesIndex::~_TexturesSamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
