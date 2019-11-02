#include <babylon/samples/collisionsandintersections/_collisions_and_intersections_samples_index.h>

#include <babylon/samples/collisionsandintersections/collisions_scene.h>
#include <babylon/samples/collisionsandintersections/intersections_scene.h>

namespace BABYLON {
namespace Samples {

_CollisionsAndIntersectionsSamplesIndex::
  _CollisionsAndIntersectionsSamplesIndex()
{
  // -- Collisions and Intersections examples --

  // Collisions Scene
  _samples["CollisionsScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<CollisionsScene>(iCanvas);
  };
  // Intersections Scene
  _samples["IntersectionsScene"] = [](ICanvas* iCanvas) {
    return std::make_unique<IntersectionsScene>(iCanvas);
  };
}

} // end of namespace Samples
} // end of namespace BABYLON
