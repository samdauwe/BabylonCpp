#ifndef BABYLON_CULLING_ICULLABLE_H
#define BABYLON_CULLING_ICULLABLE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct ICullable {
  virtual bool isInFrustum(const array_t<Plane, 6>& frustumPlanes,
                           unsigned int strategy = 0)
    = 0;
  virtual bool
  isCompletelyInFrustum(const array_t<Plane, 6>& frustumPlanes) const = 0;
}; // end of struct ICullable

} // end of namespace BABYLON

#endif // end of BABYLON_CULLING_ICULLABLE_H
