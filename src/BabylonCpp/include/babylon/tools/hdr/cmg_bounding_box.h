#ifndef BABYLON_TOOLS_HDR_CMG_BOUNDING_BOX_H
#define BABYLON_TOOLS_HDR_CMG_BOUNDING_BOX_H

#include <babylon/babylon_api.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief The bounding box information used during the conversion process.
 */
struct BABYLON_SHARED_EXPORT CMGBoundinBox {

private:
  static float MAX;
  static float MIN;

public:
  CMGBoundinBox();
  ~CMGBoundinBox();

  void clear();
  void augment(float x, float y, float z);
  void clampMin(float x, float y, float z);
  void clampMax(float x, float y, float z);
  bool empty() const;

public:
  Vector3 min;
  Vector3 max;

}; // end of struct CMGBoundinBox

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_HDR_CMG_BOUNDING_BOX_H
