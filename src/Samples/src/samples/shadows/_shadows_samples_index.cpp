#include <babylon/samples/shadows/_shadows_samples_index.h>

#include <babylon/samples/shadows/box_shadow_scene.h>
#include <babylon/samples/shadows/bulb_self_shadow_scene.h>
#include <babylon/samples/shadows/shadows_scene.h>

namespace BABYLON {
namespace Samples {

_ShadowsSamplesIndex::_ShadowsSamplesIndex()
{
  // -- Shadows examples --

  // Box Shadow Scene
  _samples["BoxShadowScene"]
    = std::make_tuple(true,                                               //
                      [](ICanvas* iCanvas) {                              //
                        return std::make_unique<BoxShadowScene>(iCanvas); //
                      });                                                 //
  // Bulf & Self Shadow Scene
  _samples["BulbSelfShadowScene"] = std::make_tuple(
    true,                                                    //
    [](ICanvas* iCanvas) {                                   //
      return std::make_unique<BulbSelfShadowScene>(iCanvas); //
    });                                                      //
  // Shadows Scene
  _samples["ShadowsScene"]
    = std::make_tuple(true,                                             //
                      [](ICanvas* iCanvas) {                            //
                        return std::make_unique<ShadowsScene>(iCanvas); //
                      });                                               //
}

_ShadowsSamplesIndex::~_ShadowsSamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
