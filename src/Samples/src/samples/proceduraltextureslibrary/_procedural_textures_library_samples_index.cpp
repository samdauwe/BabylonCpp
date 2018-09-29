#include <babylon/samples/proceduraltextureslibrary/_procedural_textures_library_samples_index.h>

#include <babylon/samples/proceduraltextureslibrary/brick_procedural_texture_scene.h>
#include <babylon/samples/proceduraltextureslibrary/cloud_procedural_texture_scene.h>
#include <babylon/samples/proceduraltextureslibrary/fire_procedural_texture_scene.h>
#include <babylon/samples/proceduraltextureslibrary/grass_procedural_texture_scene.h>
#include <babylon/samples/proceduraltextureslibrary/marble_procedural_texture_scene.h>
#include <babylon/samples/proceduraltextureslibrary/perlin_noise_procedural_texture_scene.h>
#include <babylon/samples/proceduraltextureslibrary/road_procedural_texture_scene.h>
#include <babylon/samples/proceduraltextureslibrary/starfield_procedural_texture_scene.h>
#include <babylon/samples/proceduraltextureslibrary/wood_procedural_texture_scene.h>

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
  // Cloud Procedural Texture Scene
  _samples["CloudProceduralTextureScene"] = std::make_tuple(
    true,                                                            //
    [](ICanvas* iCanvas) {                                           //
      return std::make_unique<CloudProceduralTextureScene>(iCanvas); //
    });                                                              //
  // Fire Procedural Texture Scene
  _samples["FireProceduralTextureScene"] = std::make_tuple(
    true,                                                           //
    [](ICanvas* iCanvas) {                                          //
      return std::make_unique<FireProceduralTextureScene>(iCanvas); //
    });                                                             //
  // Grass Procedural Texture Scene
  _samples["GrassProceduralTextureScene"] = std::make_tuple(
    true,                                                            //
    [](ICanvas* iCanvas) {                                           //
      return std::make_unique<GrassProceduralTextureScene>(iCanvas); //
    });                                                              //
  // Marble Procedural Texture Scene
  _samples["MarbleProceduralTextureScene"] = std::make_tuple(
    true,                                                             //
    [](ICanvas* iCanvas) {                                            //
      return std::make_unique<MarbleProceduralTextureScene>(iCanvas); //
    });                                                               //
  // Perlin Noise Procedural Texture Scene
  _samples["PerlinNoiseProceduralTextureScene"] = std::make_tuple(
    true,                                                                  //
    [](ICanvas* iCanvas) {                                                 //
      return std::make_unique<PerlinNoiseProceduralTextureScene>(iCanvas); //
    });                                                                    //
  // Road Procedural Texture Scene
  _samples["RoadProceduralTextureScene"] = std::make_tuple(
    true,                                                           //
    [](ICanvas* iCanvas) {                                          //
      return std::make_unique<RoadProceduralTextureScene>(iCanvas); //
    });                                                             //
  // Starfield Procedural Texture Scene
  _samples["StarfieldProceduralTextureScene"] = std::make_tuple(
    true,                                                                //
    [](ICanvas* iCanvas) {                                               //
      return std::make_unique<StarfieldProceduralTextureScene>(iCanvas); //
    });                                                                  //
  // Wood Procedural Texture Scene
  _samples["WoodProceduralTextureScene"] = std::make_tuple(
    true,                                                           //
    [](ICanvas* iCanvas) {                                          //
      return std::make_unique<WoodProceduralTextureScene>(iCanvas); //
    });                                                             //
}

_ProceduralTexturesLibrarySamplesIndex::
  ~_ProceduralTexturesLibrarySamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
