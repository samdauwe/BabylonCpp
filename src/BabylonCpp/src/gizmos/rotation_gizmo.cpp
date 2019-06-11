#include <babylon/gizmos/rotation_gizmo.h>

#include <babylon/behaviors/meshes/pointer_drag_behavior.h>
#include <babylon/core/logging.h>
#include <babylon/gizmos/plane_rotation_gizmo.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

RotationGizmo::RotationGizmo(const UtilityLayerRendererPtr& iGizmoLayer,
                             unsigned int tessellation)
    : Gizmo{iGizmoLayer}
    , snapDistance{this, &RotationGizmo::get_snapDistance,
                   &RotationGizmo::set_snapDistance}
{
  xGizmo = std::make_unique<PlaneRotationGizmo>(Vector3(1.f, 0.f, 0.f),
                                                Color3::Red().scale(0.5f),
                                                iGizmoLayer, tessellation);
  yGizmo = std::make_unique<PlaneRotationGizmo>(Vector3(0.f, 1.f, 0.f),
                                                Color3::Green().scale(0.5f),
                                                iGizmoLayer, tessellation);
  zGizmo = std::make_unique<PlaneRotationGizmo>(Vector3(0.f, 0.f, 1.f),
                                                Color3::Blue().scale(0.5f),
                                                iGizmoLayer, tessellation);

  // Relay drag events
  for (const auto& gizmo : {xGizmo.get(), yGizmo.get(), zGizmo.get()}) {
    gizmo->dragBehavior->onDragStartObservable.add(
      [&](DragStartOrEndEvent* /*event*/, EventState& /*es*/) {
        onDragStartObservable.notifyObservers(nullptr);
      });
    gizmo->dragBehavior->onDragEndObservable.add(
      [&](DragStartOrEndEvent* /*event*/, EventState& /*es*/) {
        onDragEndObservable.notifyObservers(nullptr);
      });
  }

  attachedMesh = nullptr;
}

RotationGizmo::~RotationGizmo()
{
}

AbstractMeshPtr& RotationGizmo::get_attachedMesh()
{
  return xGizmo->attachedMesh;
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
  onDragStartObservable.clear();
  onDragEndObservable.clear();
}

void RotationGizmo::setCustomMesh(const MeshPtr& /*mesh*/,
                                  bool /*useGizmoMaterial*/)
{
  BABYLON_LOG_ERROR("RotationGizmo",
                    "Custom meshes are not supported on this gizmo, please set "
                    "the custom meshes on the gizmos contained within this one "
                    "(gizmo.xGizmo, gizmo.yGizmo, gizmo.zGizmo)")
}

} // end of namespace BABYLON
