#include <babylon/samples/optimizations/_optimizations_samples_index.h>

#include <babylon/samples/optimizations/box_instances_scene.h>
#include <babylon/samples/optimizations/building_instances_grid_scene.h>

namespace BABYLON {
namespace Samples {

_OptimizationsSamplesIndex::_OptimizationsSamplesIndex()
{
  // -- Optimizations examples --

  // Box Instances Scene
  _samples["BoxInstancesScene"]
    = std::make_tuple(true,                                                  //
                      [](ICanvas* iCanvas) {                                 //
                        return std::make_unique<BoxInstancesScene>(iCanvas); //
                      });                                                    //
  // Building Instances Grid Scene
  _samples["BuildingInstancesGridScene"] = std::make_tuple(
    true,                                                           //
    [](ICanvas* iCanvas) {                                          //
      return std::make_unique<BuildingInstancesGridScene>(iCanvas); //
    });                                                             //
}

_OptimizationsSamplesIndex::~_OptimizationsSamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
