#ifndef BABYLON_SAMPLES_COLLISIONS_AND_INTERSECTIONS_SAMPLES_INDEX_H
#define BABYLON_SAMPLES_COLLISIONS_AND_INTERSECTIONS_SAMPLES_INDEX_H

#include <babylon/samples/samples_common.h>

namespace BABYLON {
namespace Samples {

class _CollisionsAndIntersectionsSamplesIndex : public _ISamplesIndex {

public:
  /**
   * @brief Returns the category name.
   * @return the category name
   */
  static const std::string CategoryName()
  {
    return "Collisions and Intersections";
  }

public:
  _CollisionsAndIntersectionsSamplesIndex();
  virtual ~_CollisionsAndIntersectionsSamplesIndex() = default;

}; // end of class _CollisionsAndIntersectionsSamplesIndex

} // end of namespace Samples
} // end of namespace BABYLON

#endif // end of BABYLON_SAMPLES_COLLISIONS_AND_INTERSECTIONS_SAMPLES_INDEX_H
