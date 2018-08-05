#include <babylon/gizmos/scale_gizmo.h>

#include <babylon/gizmos/axis_scale_gizmo.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

ScaleGizmo::ScaleGizmo(const shared_ptr_t<UtilityLayerRenderer>& iGizmoLayer)
    : Gizmo{iGizmoLayer}
    , xGizmo{::std::make_unique<AxisScaleGizmo>(
        Vector3(1.f, 0.f, 0.f), Color3::Green().scale(0.5f), iGizmoLayer)}
    , yGizmo{::std::make_unique<AxisScaleGizmo>(
        Vector3(0.f, 1.f, 0.f), Color3::Red().scale(0.5f), iGizmoLayer)}
    , zGizmo{::std::make_unique<AxisScaleGizmo>(
        Vector3(0.f, 0.f, 1.f), Color3::Blue().scale(0.5f), iGizmoLayer)}
    , updateGizmoRotationToMatchAttachedMesh{
        this, &ScaleGizmo::get_updateGizmoRotationToMatchAttachedMesh,
        &ScaleGizmo::set_updateGizmoRotationToMatchAttachedMesh}
{
  attachedMesh = nullptr;
}

ScaleGizmo::~ScaleGizmo()
{
}

void ScaleGizmo::set_attachedMesh(AbstractMesh* const& mesh)
{
  if (xGizmo) {
    xGizmo->attachedMesh = mesh;
    yGizmo->attachedMesh = mesh;
    zGizmo->attachedMesh = mesh;
  }
}

void ScaleGizmo::set_updateGizmoRotationToMatchAttachedMesh(bool value)
{
  if (xGizmo) {
    xGizmo->updateGizmoRotationToMatchAttachedMesh = value;
    yGizmo->updateGizmoRotationToMatchAttachedMesh = value;
    zGizmo->updateGizmoRotationToMatchAttachedMesh = value;
  }
}

bool ScaleGizmo::get_updateGizmoRotationToMatchAttachedMesh() const
{
  return xGizmo->updateGizmoRotationToMatchAttachedMesh;
}

void ScaleGizmo::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  xGizmo->dispose(doNotRecurse, disposeMaterialAndTextures);
  yGizmo->dispose(doNotRecurse, disposeMaterialAndTextures);
  zGizmo->dispose(doNotRecurse, disposeMaterialAndTextures);
}

} // end of namespace BABYLON
