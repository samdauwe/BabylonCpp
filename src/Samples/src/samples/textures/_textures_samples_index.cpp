#include <babylon/samples/textures/_textures_samples_index.h>

#include <babylon/samples/textures/bump_texture_scene.h>
#include <babylon/samples/textures/bump_texture_spheres_scene.h>
#include <babylon/samples/textures/custom_render_targets_scene.h>
#include <babylon/samples/textures/image_texture_scene.h>
#include <babylon/samples/textures/mirrors_scene.h>
#include <babylon/samples/textures/procedural_textures_scene.h>
#include <babylon/samples/textures/render_target_texture_scene.h>

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
  // Custom Render Targets Scene
  _samples["CustomRenderTargetsScene"] = std::make_tuple(
    true,                                                         //
    [](ICanvas* iCanvas) {                                        //
      return std::make_unique<CustomRenderTargetsScene>(iCanvas); //
    });                                                           //
  // Image Textures Scene
  _samples["ImageTextureScene"]
    = std::make_tuple(true,                                                  //
                      [](ICanvas* iCanvas) {                                 //
                        return std::make_unique<ImageTextureScene>(iCanvas); //
                      });                                                    //
  // Mirrors Scene
  _samples["MirrorsScene"]
    = std::make_tuple(true,                                             //
                      [](ICanvas* iCanvas) {                            //
                        return std::make_unique<MirrorsScene>(iCanvas); //
                      });                                               //
  // Procedural Textures Scene
  _samples["ProceduralTexturesScene"] = std::make_tuple(
    false,                                                       //
    [](ICanvas* iCanvas) {                                       //
      return std::make_unique<ProceduralTexturesScene>(iCanvas); //
    });                                                          //
  // Render Target Texture Scene
  _samples["RenderTargetTextureScene"] = std::make_tuple(
    true,                                                         //
    [](ICanvas* iCanvas) {                                        //
      return std::make_unique<RenderTargetTextureScene>(iCanvas); //
    });                                                           //
}

_TexturesSamplesIndex::~_TexturesSamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
