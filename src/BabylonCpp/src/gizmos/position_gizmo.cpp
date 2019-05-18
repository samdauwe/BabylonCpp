#include <babylon/gizmos/position_gizmo.h>

#include <babylon/behaviors/meshes/pointer_drag_behavior.h>
#include <babylon/core/logging.h>
#include <babylon/gizmos/axis_drag_gizmo.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector3.h>

namespace BABYLON {

PositionGizmo::PositionGizmo(
  const std::shared_ptr<UtilityLayerRenderer>& iGizmoLayer)
    : Gizmo{iGizmoLayer}
    , xGizmo{std::make_unique<AxisDragGizmo>(
        Vector3(1.f, 0.f, 0.f), Color3::Red().scale(0.5f), iGizmoLayer)}
    , yGizmo{std::make_unique<AxisDragGizmo>(
        Vector3(0.f, 1.f, 0.f), Color3::Green().scale(0.5f), iGizmoLayer)}
    , zGizmo{std::make_unique<AxisDragGizmo>(
        Vector3(0.f, 0.f, 1.f), Color3::Blue().scale(0.5f), iGizmoLayer)}
    , snapDistance{this, &PositionGizmo::get_snapDistance,
                   &PositionGizmo::set_snapDistance}
{
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

PositionGizmo::~PositionGizmo()
{
}

void PositionGizmo::set_attachedMesh(AbstractMesh* const& mesh)
{
  if (xGizmo) {
    xGizmo->attachedMesh = mesh;
    yGizmo->attachedMesh = mesh;
    zGizmo->attachedMesh = mesh;
  }
}

void PositionGizmo::set_updateGizmoRotationToMatchAttachedMesh(bool value)
{
  if (xGizmo) {
    xGizmo->updateGizmoRotationToMatchAttachedMesh = value;
    yGizmo->updateGizmoRotationToMatchAttachedMesh = value;
    zGizmo->updateGizmoRotationToMatchAttachedMesh = value;
  }
}

bool PositionGizmo::get_updateGizmoRotationToMatchAttachedMesh() const
{
  return xGizmo->updateGizmoRotationToMatchAttachedMesh;
}

void PositionGizmo::set_snapDistance(float value)
{
  if (xGizmo) {
    xGizmo->snapDistance = value;
    yGizmo->snapDistance = value;
    zGizmo->snapDistance = value;
  }
}

float PositionGizmo::get_snapDistance() const
{
  return xGizmo->snapDistance;
}

void PositionGizmo::set_scaleRatio(float value)
{
  if (xGizmo) {
    xGizmo->scaleRatio = value;
    yGizmo->scaleRatio = value;
    zGizmo->scaleRatio = value;
  }
}

float PositionGizmo::get_scaleRatio() const
{
  return xGizmo->scaleRatio;
}

void PositionGizmo::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  xGizmo->dispose(doNotRecurse, disposeMaterialAndTextures);
  yGizmo->dispose(doNotRecurse, disposeMaterialAndTextures);
  zGizmo->dispose(doNotRecurse, disposeMaterialAndTextures);
  onDragStartObservable.clear();
  onDragEndObservable.clear();
}

void PositionGizmo::setCustomMesh(const MeshPtr& /*mesh*/,
                                  bool /*useGizmoMaterial*/)
{
  BABYLON_LOG_ERROR("PositionGizmo",
                    "Custom meshes are not supported on this gizmo, please set "
                    "the custom meshes on the gizmos contained within this one "
                    "(gizmo.xGizmo, gizmo.yGizmo, gizmo.zGizmo)");
}

} // end of namespace BABYLON
