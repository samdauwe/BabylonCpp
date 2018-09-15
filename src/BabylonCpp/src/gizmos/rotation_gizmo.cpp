#include <babylon/gizmos/rotation_gizmo.h>

#include <babylon/core/logging.h>
#include <babylon/gizmos/plane_rotation_gizmo.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

RotationGizmo::RotationGizmo(
  const std::shared_ptr<UtilityLayerRenderer>& iGizmoLayer)
    : Gizmo{iGizmoLayer}
    , xGizmo{::std::make_unique<PlaneRotationGizmo>(
        Vector3(1.f, 0.f, 0.f), Color3::Green().scale(0.5f), iGizmoLayer)}
    , yGizmo{::std::make_unique<PlaneRotationGizmo>(
        Vector3(0.f, 1.f, 0.f), Color3::Red().scale(0.5f), iGizmoLayer)}
    , zGizmo{::std::make_unique<PlaneRotationGizmo>(
        Vector3(0.f, 0.f, 1.f), Color3::Blue().scale(0.5f), iGizmoLayer)}
    , snapDistance{this, &RotationGizmo::get_snapDistance,
                   &RotationGizmo::set_snapDistance}
    , scaleRatio{this, &RotationGizmo::get_scaleRatio,
                 &RotationGizmo::set_scaleRatio}
    , updateGizmoRotationToMatchAttachedMesh{
        this, &RotationGizmo::get_updateGizmoRotationToMatchAttachedMesh,
        &RotationGizmo::set_updateGizmoRotationToMatchAttachedMesh}
{
  attachedMesh = nullptr;
}

RotationGizmo::~RotationGizmo()
{
}

void RotationGizmo::set_attachedMesh(const AbstractMeshPtr& mesh)
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

void RotationGizmo::set_snapDistance(float value)
{
  if (xGizmo) {
    xGizmo->snapDistance = value;
    yGizmo->snapDistance = value;
    zGizmo->snapDistance = value;
  }
}

float RotationGizmo::get_snapDistance() const
{
  return xGizmo->snapDistance;
}

void RotationGizmo::set_scaleRatio(float value)
{
  if (xGizmo) {
    xGizmo->scaleRatio = value;
    yGizmo->scaleRatio = value;
    zGizmo->scaleRatio = value;
  }
}

float RotationGizmo::get_scaleRatio() const
{
  return xGizmo->scaleRatio;
}

void RotationGizmo::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  xGizmo->dispose(doNotRecurse, disposeMaterialAndTextures);
  yGizmo->dispose(doNotRecurse, disposeMaterialAndTextures);
  zGizmo->dispose(doNotRecurse, disposeMaterialAndTextures);
}

void RotationGizmo::setCustomMesh(const MeshPtr& /*mesh*/,
                                  bool /*useGizmoMaterial*/)
{
  BABYLON_LOG_ERROR("RotationGizmo",
                    "Custom meshes are not supported on this gizmo, please set "
                    "the custom meshes on the gizmos contained within this one "
                    "(gizmo.xGizmo, gizmo.yGizmo, gizmo.zGizmo)");
}

} // end of namespace BABYLON
