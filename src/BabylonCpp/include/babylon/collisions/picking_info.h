#ifndef BABYLON_COLLISIONS_PICKING_INFO_H
#define BABYLON_COLLISIONS_PICKING_INFO_H

#include <babylon/babylon_global.h>
#include <babylon/culling/ray.h>
#include <babylon/math/vector3.h>

#include <babylon/core/nullable.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT PickingInfo {

public:
  PickingInfo();
  ~PickingInfo();

  /** Methods **/
  Nullable<Vector3> getNormal(bool useWorldCoordinates = false,
                              bool useVerticesNormals  = true);
  Nullable<Vector2> getTextureCoordinates();

public:
  bool hit;
  float distance;
  Nullable<Vector3> pickedPoint;
  AbstractMesh* pickedMesh;
  float bu;
  float bv;
  unsigned int faceId;
  unsigned int subMeshId;
  Sprite* pickedSprite;
  Nullable<Ray> ray;

}; // end of class PickingInfo

} // end of namespace BABYLON

#endif // end of BABYLON_COLLISIONS_PICKING_INFO_H
