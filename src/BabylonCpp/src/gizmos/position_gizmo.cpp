#include <babylon/gizmos/position_gizmo.h>

#include <babylon/gizmos/axis_drag_gizmo.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

PositionGizmo::PositionGizmo(UtilityLayerRenderer* iGizmoLayer)
    : Gizmo{iGizmoLayer}
    , updateGizmoRotationToMatchAttachedMesh{this,
                                             &PositionGizmo::
                                               get_updateGizmoRotationToMatchAttachedMesh,
                                             &PositionGizmo::
                                               set_updateGizmoRotationToMatchAttachedMesh}
    , _xDrag{::std::make_unique<AxisDragGizmo>(
        iGizmoLayer, Vector3(1.f, 0.f, 0.f), Color3::Green().scale(0.5f))}
    , _yDrag{::std::make_unique<AxisDragGizmo>(
        iGizmoLayer, Vector3(0.f, 1.f, 0.f), Color3::Red().scale(0.5f))}
    , _zDrag{::std::make_unique<AxisDragGizmo>(
        iGizmoLayer, Vector3(0.f, 0.f, 1.f), Color3::Blue().scale(0.5f))}
{
  attachedMesh = nullptr;
}

PositionGizmo::~PositionGizmo()
{
}

void PositionGizmo::set_attachedMesh(AbstractMesh* const& mesh)
{
  if (_xDrag) {
    _xDrag->attachedMesh = mesh;
    _yDrag->attachedMesh = mesh;
    _zDrag->attachedMesh = mesh;
  }
}

void PositionGizmo::set_updateGizmoRotationToMatchAttachedMesh(bool value)
{
  if (_xDrag) {
    _xDrag->updateGizmoRotationToMatchAttachedMesh = value;
    _yDrag->updateGizmoRotationToMatchAttachedMesh = value;
    _zDrag->updateGizmoRotationToMatchAttachedMesh = value;
  }
}

bool PositionGizmo::get_updateGizmoRotationToMatchAttachedMesh() const
{
  return _xDrag->updateGizmoRotationToMatchAttachedMesh;
}

void PositionGizmo::dispose(bool /*doNotRecurse*/,
                            bool /*disposeMaterialAndTextures*/)
{
  _xDrag->dispose();
  _yDrag->dispose();
  _zDrag->dispose();
}

} // end of namespace BABYLON
