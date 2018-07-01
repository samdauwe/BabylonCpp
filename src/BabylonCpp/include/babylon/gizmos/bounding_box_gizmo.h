#ifndef BABYLON_GIZMOS_BOUNDING_BOX_GIZMO_H
#define BABYLON_GIZMOS_BOUNDING_BOX_GIZMO_H

#include <babylon/babylon_global.h>
#include <babylon/gizmos/gizmo.h>
#include <babylon/math/quaternion.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

/**
 * @brief Bounding box gizmo.
 */
class BABYLON_SHARED_EXPORT BoundingBoxGizmo : public Gizmo {

public:
  /**
   * @brief Creates an BoundingBoxGizmo.
   * @param gizmoLayer The utility layer the gizmo will be added to
   * @param color The color of the gizmo
   */
  BoundingBoxGizmo(UtilityLayerRenderer* gizmoLayer, const Color3& color);
  ~BoundingBoxGizmo() override;

  /**
   * @brief Enables rotation on the specified axis and disables rotation on the
   * others.
   * @param axis The list of axis that should be enabled (eg. "xy" or "xyz")
   */
  void setEnabledRotationAxis(const string_t axis);

  /**
   * @brief Disposes of the gizmo.
   */
  void dispose(bool doNotRecurse               = false,
               bool disposeMaterialAndTextures = false) override;

private:
  void _selectNode(Mesh* selectedMesh);
  void _updateBoundingBox();

private:
  AbstractMesh* _lineBoundingBox;
  AbstractMesh* _rotateSpheresParent;
  AbstractMesh* _scaleBoxesParent;
  Vector3 _boundingDimensions;
  Observer<Scene>::Ptr _renderObserver;
  Observer<PointerInfo>::Ptr _pointerObserver;
  float _scaleDragSpeed;

  Quaternion _tmpQuaternion;
  Vector3 _tmpVector;

}; // end of class BoundingBoxGizmo

} // end of namespace BABYLON

#endif // end of BABYLON_GIZMOS_BOUNDING_BOX_GIZMO_H
