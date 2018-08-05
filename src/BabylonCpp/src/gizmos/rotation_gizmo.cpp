#include <babylon/gizmos/rotation_gizmo.h>

#include <babylon/core/logging.h>
#include <babylon/gizmos/plane_rotation_gizmo.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

RotationGizmo::RotationGizmo(
  const shared_ptr_t<UtilityLayerRenderer>& iGizmoLayer)
    : Gizmo{iGizmoLayer}
    , xGizmo{::std::make_unique<PlaneRotationGizmo>(
        Vector3(1.f, 0.f, 0.f), Color3::Green().scale(0.5f), iGizmoLayer)}
    , yGizmo{::std::make_unique<PlaneRotationGizmo>(
        Vector3(0.f, 1.f, 0.f), Color3::Red().scale(0.5f), iGizmoLayer)}
    , zGizmo{::std::make_unique<PlaneRotationGizmo>(
        Vector3(0.f, 0.f, 1.f), Color3::Blue().scale(0.5f), iGizmoLayer)}
    , updateGizmoRotationToMatchAttachedMesh{
        this, &RotationGizmo::get_updateGizmoRotationToMatchAttachedMesh,
        &RotationGizmo::set_updateGizmoRotationToMatchAttachedMesh}
{
  attachedMesh = nullptr;
}

RotationGizmo::~RotationGizmo()
{
}

void RotationGizmo::set_attachedMesh(AbstractMesh* const& mesh)
{
  if (xGizmo) {
    xGizmo->attachedMesh = mesh;
    yGizmo->attachedMesh = mesh;
    zGizmo->attachedMesh = mesh;
  }
}

void RotationGizmo::set_updateGizmoRotationToMatchAttachedMesh(bool value)
{
  if (xGizmo) {
    xGizmo->updateGizmoRotationToMatchAttachedMesh = value;
    yGizmo->updateGizmoRotationToMatchAttachedMesh = value;
    zGizmo->updateGizmoRotationToMatchAttachedMesh = value;
  }
}

bool RotationGizmo::get_updateGizmoRotationToMatchAttachedMesh() const
{
  return xGizmo->updateGizmoRotationToMatchAttachedMesh;
}

void RotationGizmo::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  xGizmo->dispose(doNotRecurse, disposeMaterialAndTextures);
  yGizmo->dispose(doNotRecurse, disposeMaterialAndTextures);
  zGizmo->dispose(doNotRecurse, disposeMaterialAndTextures);
}

void RotationGizmo::setCustomMesh(Mesh* /*mesh*/)
{
  BABYLON_LOG_ERROR("RotationGizmo",
                    "Custom meshes are not supported on this gizmo, please set "
                    "the custom meshes on the gizmos contained within this one "
                    "(gizmo.xGizmo, gizmo.yGizmo, gizmo.zGizmo)");
}

} // end of namespace BABYLON
