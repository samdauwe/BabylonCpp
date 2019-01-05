#include <babylon/samples/textures/_textures_samples_index.h>

#include <babylon/samples/textures/blur_mode_for_mirrors_scene.h>
#include <babylon/samples/textures/bump_texture_scene.h>
#include <babylon/samples/textures/bump_texture_spheres_scene.h>
#include <babylon/samples/textures/cube_texture_different_faces_scene.h>
#include <babylon/samples/textures/custom_render_targets_scene.h>
#include <babylon/samples/textures/environment_texture_scene.h>
#include <babylon/samples/textures/equirectangular_map_as_reflection_texture_scene.h>
#include <babylon/samples/textures/hdr_cube_texture_scene.h>
#include <babylon/samples/textures/image_texture_scene.h>
#include <babylon/samples/textures/mirror_texture_scene.h>
#include <babylon/samples/textures/mirrors_scene.h>
#include <babylon/samples/textures/multi_sample_render_targets_scene.h>
#include <babylon/samples/textures/opacity_texture_scene.h>
#include <babylon/samples/textures/photo_dome_scene.h>
#include <babylon/samples/textures/procedural_textures_scene.h>
#include <babylon/samples/textures/render_target_texture_scene.h>
#include <babylon/samples/textures/sphere_textures_scene.h>
#include <babylon/samples/textures/sphere_transparent_textures_scene.h>
#include <babylon/samples/textures/spherical_reflection_texture_scene.h>

namespace BABYLON {
namespace Samples {

_TexturesSamplesIndex::_TexturesSamplesIndex()
{
  // -- Textures examples --

  // Blur Mode For Mirrors Scene
  _samples["BlurModeForMirrorsScene"] = std::make_tuple(
    true,                                                        //
    [](ICanvas* iCanvas) {                                       //
      return std::make_unique<BlurModeForMirrorsScene>(iCanvas); //
    });                                                          //
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
  // Cube texture different faces scene
  _samples["CubeTextureDifferentFacesScene"] = std::make_tuple(
    true,                                                               //
    [](ICanvas* iCanvas) {                                              //
      return std::make_unique<CubeTextureDifferentFacesScene>(iCanvas); //
    });                                                                 //
  // Custom Render Targets Scene
  _samples["CustomRenderTargetsScene"] = std::make_tuple(
    true,                                                         //
    [](ICanvas* iCanvas) {                                        //
      return std::make_unique<CustomRenderTargetsScene>(iCanvas); //
    });                                                           //
  // Environment Texture Scene
  _samples["EnvironmentTextureScene"] = std::make_tuple(
    false,                                                       //
    [](ICanvas* iCanvas) {                                       //
      return std::make_unique<EnvironmentTextureScene>(iCanvas); //
    });                                                          //
  // Equirectangular Map as Reflection Texture Scene
  _samples["EquirectangularMapAsReflectionTextureScene"] = std::make_tuple(
    true,                  //
    [](ICanvas* iCanvas) { //
      return std::make_unique<EquirectangularMapAsReflectionTextureScene>(
        iCanvas); //
    });           //
  // High Dynamic Range (HDR) cube texture scene
  _samples["HDRCubeTextureScene"] = std::make_tuple(
    true,                                                    //
    [](ICanvas* iCanvas) {                                   //
      return std::make_unique<HDRCubeTextureScene>(iCanvas); //
    });                                                      //
  // Image Textures Scene
  _samples["ImageTextureScene"]
    = std::make_tuple(true,                                                  //
                      [](ICanvas* iCanvas) {                                 //
                        return std::make_unique<ImageTextureScene>(iCanvas); //
                      });                                                    //
  // Mirror Textures Scene
  _samples["MirrorTextureScene"]
    = std::make_tuple(true,                                                   //
                      [](ICanvas* iCanvas) {                                  //
                        return std::make_unique<MirrorTextureScene>(iCanvas); //
                      });                                                     //
  // Mirrors Scene
  _samples["MirrorsScene"]
    = std::make_tuple(true,                                             //
                      [](ICanvas* iCanvas) {                            //
                        return std::make_unique<MirrorsScene>(iCanvas); //
                      });                                               //
  // Multisample Render Targets Scene
  _samples["MultiSampleRenderTargetsScene"] = std::make_tuple(
    true,                                                              //
    [](ICanvas* iCanvas) {                                             //
      return std::make_unique<MultiSampleRenderTargetsScene>(iCanvas); //
    });                                                                //
  // Opacity Textur Scene
  _samples["OpacityTexturScene"]
    = std::make_tuple(true,                                                   //
                      [](ICanvas* iCanvas) {                                  //
                        return std::make_unique<OpacityTexturScene>(iCanvas); //
                      });                                                     //
  // Photo Dome Scene
  _samples["PhotoDomeScene"]
    = std::make_tuple(false,                                              //
                      [](ICanvas* iCanvas) {                              //
                        return std::make_unique<PhotoDomeScene>(iCanvas); //
                      });                                                 //
  // Procedural Textures Scene
  _samples["ProceduralTexturesScene"] = std::make_tuple(
    true,                                                        //
    [](ICanvas* iCanvas) {                                       //
      return std::make_unique<ProceduralTexturesScene>(iCanvas); //
    });                                                          //
  // Render Target Texture Scene
  _samples["RenderTargetTextureScene"] = std::make_tuple(
    true,                                                         //
    [](ICanvas* iCanvas) {                                        //
      return std::make_unique<RenderTargetTextureScene>(iCanvas); //
    });                                                           //
  // Sphere Textures Scene
  _samples["SphereTexturesScene"] = std::make_tuple(
    true,                                                    //
    [](ICanvas* iCanvas) {                                   //
      return std::make_unique<SphereTexturesScene>(iCanvas); //
    });                                                      //
  // Sphere Transparent Textures Scene
  _samples["SphereTransparentTexturesScene"] = std::make_tuple(
    true,                                                               //
    [](ICanvas* iCanvas) {                                              //
      return std::make_unique<SphereTransparentTexturesScene>(iCanvas); //
    });                                                                 //
  // Spherical Reflection Texture Scene
  _samples["SphericalReflectionTextureScene"] = std::make_tuple(
    true,                                                                //
    [](ICanvas* iCanvas) {                                               //
      return std::make_unique<SphericalReflectionTextureScene>(iCanvas); //
    });                                                                  //
}

_TexturesSamplesIndex::~_TexturesSamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
