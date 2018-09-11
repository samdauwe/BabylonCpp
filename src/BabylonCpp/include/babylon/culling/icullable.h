#ifndef BABYLON_CULLING_ICULLABLE_H
#define BABYLON_CULLING_ICULLABLE_H

#include <array>

#include <babylon/babylon_api.h>

namespace BABYLON {

class Plane;

struct ICullable {
  virtual bool isInFrustum(const std::array<Plane, 6>& frustumPlanes,
                           unsigned int strategy = 0)
    = 0;
  virtual bool
  isCompletelyInFrustum(const std::array<Plane, 6>& frustumPlanes) const = 0;
}; // end of struct ICullable

} // end of namespace BABYLON

#endif // end of BABYLON_CULLING_ICULLABLE_H
