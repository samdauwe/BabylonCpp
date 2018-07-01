#include <babylon/gizmos/rotation_gizmo.h>

#include <babylon/gizmos/plane_rotation_gizmo.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

RotationGizmo::RotationGizmo(UtilityLayerRenderer* gizmoLayer)
    : Gizmo{gizmoLayer}
    , updateGizmoRotationToMatchAttachedMesh{this,
                                             &RotationGizmo::
                                               get_updateGizmoRotationToMatchAttachedMesh,
                                             &RotationGizmo::
                                               set_updateGizmoRotationToMatchAttachedMesh}
    , _xDrag{::std::make_unique<PlaneRotationGizmo>(
        gizmoLayer, Vector3(1.f, 0.f, 0.f), Color3::Green().scale(0.5f))}
    , _yDrag{::std::make_unique<PlaneRotationGizmo>(
        gizmoLayer, Vector3(0.f, 1.f, 0.f), Color3::Red().scale(0.5f))}
    , _zDrag{::std::make_unique<PlaneRotationGizmo>(
        gizmoLayer, Vector3(0.f, 0.f, 1.f), Color3::Blue().scale(0.5f))}
{
  attachedMesh = nullptr;
}

RotationGizmo::~RotationGizmo()
{
}

void RotationGizmo::set_attachedMesh(AbstractMesh* const& mesh)
{
  if (_xDrag) {
    _xDrag->attachedMesh = mesh;
    _yDrag->attachedMesh = mesh;
    _zDrag->attachedMesh = mesh;
  }
}

void RotationGizmo::set_updateGizmoRotationToMatchAttachedMesh(bool value)
{
  if (_xDrag) {
    _xDrag->updateGizmoRotationToMatchAttachedMesh = value;
    _yDrag->updateGizmoRotationToMatchAttachedMesh = value;
    _zDrag->updateGizmoRotationToMatchAttachedMesh = value;
  }
}

bool RotationGizmo::get_updateGizmoRotationToMatchAttachedMesh() const
{
  return _xDrag->updateGizmoRotationToMatchAttachedMesh;
}

void RotationGizmo::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  _xDrag->dispose(doNotRecurse, disposeMaterialAndTextures);
  _yDrag->dispose(doNotRecurse, disposeMaterialAndTextures);
  _zDrag->dispose(doNotRecurse, disposeMaterialAndTextures);
}

} // end of namespace BABYLON
