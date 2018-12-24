#include <babylon/samples/collisionsandintersections/_collisions_and_intersections_samples_index.h>

#include <babylon/samples/collisionsandintersections/intersections_scene.h>

namespace BABYLON {
namespace Samples {

_CollisionsAndIntersectionsSamplesIndex::
  _CollisionsAndIntersectionsSamplesIndex()
{
  // -- Collisions and Intersections examples --

  // Intersections Scene
  _samples["IntersectionsScene"]
    = std::make_tuple(true,                                                   //
                      [](ICanvas* iCanvas) {                                  //
                        return std::make_unique<IntersectionsScene>(iCanvas); //
                      });                                                     //
}

_CollisionsAndIntersectionsSamplesIndex::
  ~_CollisionsAndIntersectionsSamplesIndex()
{
}

} // end of namespace Samples
} // end of namespace BABYLON
