#include <babylon/gizmos/scale_gizmo.h>

#include <babylon/gizmos/axis_scale_gizmo.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

ScaleGizmo::ScaleGizmo(UtilityLayerRenderer* gizmoLayer)
    : Gizmo{gizmoLayer}
    , updateGizmoRotationToMatchAttachedMesh{this,
                                             &ScaleGizmo::
                                               get_updateGizmoRotationToMatchAttachedMesh,
                                             &ScaleGizmo::
                                               set_updateGizmoRotationToMatchAttachedMesh}
    , _xDrag{::std::make_unique<AxisScaleGizmo>(
        gizmoLayer, Vector3(1.f, 0.f, 0.f), Color3::Green().scale(0.5f))}
    , _yDrag{::std::make_unique<AxisScaleGizmo>(
        gizmoLayer, Vector3(0.f, 1.f, 0.f), Color3::Red().scale(0.5f))}
    , _zDrag{::std::make_unique<AxisScaleGizmo>(
        gizmoLayer, Vector3(0.f, 0.f, 1.f), Color3::Blue().scale(0.5f))}
{
  attachedMesh = nullptr;
}

ScaleGizmo::~ScaleGizmo()
{
}

void ScaleGizmo::set_attachedMesh(AbstractMesh* const& mesh)
{
  if (_xDrag) {
    _xDrag->attachedMesh = mesh;
    _yDrag->attachedMesh = mesh;
    _zDrag->attachedMesh = mesh;
  }
}

void ScaleGizmo::set_updateGizmoRotationToMatchAttachedMesh(bool value)
{
  if (_xDrag) {
    _xDrag->updateGizmoRotationToMatchAttachedMesh = value;
    _yDrag->updateGizmoRotationToMatchAttachedMesh = value;
    _zDrag->updateGizmoRotationToMatchAttachedMesh = value;
  }
}

bool ScaleGizmo::get_updateGizmoRotationToMatchAttachedMesh() const
{
  return _xDrag->updateGizmoRotationToMatchAttachedMesh;
}

void ScaleGizmo::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  _xDrag->dispose(doNotRecurse, disposeMaterialAndTextures);
  _yDrag->dispose(doNotRecurse, disposeMaterialAndTextures);
  _zDrag->dispose(doNotRecurse, disposeMaterialAndTextures);
}

} // end of namespace BABYLON
