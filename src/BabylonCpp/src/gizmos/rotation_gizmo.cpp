#include <babylon/gizmos/rotation_gizmo.h>

#include <babylon/behaviors/meshes/pointer_drag_behavior.h>
#include <babylon/core/logging.h>
#include <babylon/engines/scene.h>
#include <babylon/gizmos/gizmo_manager.h>
#include <babylon/gizmos/plane_rotation_gizmo.h>
#include <babylon/gizmos/position_gizmo.h>
#include <babylon/gizmos/scale_gizmo.h>
#include <babylon/maths/color3.h>
#include <babylon/maths/vector3.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/gizmos/bounding_box_gizmo.h>

namespace BABYLON {

RotationGizmo::RotationGizmo(const UtilityLayerRendererPtr& iGizmoLayer, unsigned int tessellation,
                             bool useEulerRotation, float thickness, GizmoManager* gizmoManager)
    : Gizmo{iGizmoLayer}
    , snapDistance{this, &RotationGizmo::get_snapDistance, &RotationGizmo::set_snapDistance}
{
  xGizmo = std::make_unique<PlaneRotationGizmo>(Vector3(1.f, 0.f, 0.f), Color3::Red().scale(0.5f),
                                                iGizmoLayer, tessellation, this, useEulerRotation,
                                                thickness);
  yGizmo = std::make_unique<PlaneRotationGizmo>(Vector3(0.f, 1.f, 0.f), Color3::Green().scale(0.5f),
                                                iGizmoLayer, tessellation, this, useEulerRotation,
                                                thickness);
  zGizmo = std::make_unique<PlaneRotationGizmo>(Vector3(0.f, 0.f, 1.f), Color3::Blue().scale(0.5f),
                                                iGizmoLayer, tessellation, this, useEulerRotation,
                                                thickness);

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
  attachedNode = nullptr;

  if (gizmoManager) {
    gizmoManager->addToAxisCache(_gizmoAxisCache);
  }
  else {
    // Only subscribe to pointer event if gizmoManager isnt
    Gizmo::GizmoAxisPointerObserver(gizmoLayer, _gizmoAxisCache);
  }
}

RotationGizmo::~RotationGizmo() = default;

AbstractMeshPtr& RotationGizmo::get_attachedMesh()
{
  return _meshAttached;
}

void RotationGizmo::set_attachedMesh(const AbstractMeshPtr& mesh)
{
  _meshAttached = mesh;
  _nodeAttached = mesh;
  _checkBillboardTransform();
  for (const auto& gizmo : {xGizmo.get(), yGizmo.get(), zGizmo.get()}) {
    if (gizmo->isEnabled()) {
      gizmo->attachedMesh = mesh;
    }
    else {
      gizmo->attachedMesh = nullptr;
    }
  }
}

NodePtr& RotationGizmo::get_attachedNode()
{
  return _nodeAttached;
}

void RotationGizmo::set_attachedNode(const NodePtr& node)
{
  _meshAttached = nullptr;
  _nodeAttached = node;
  _checkBillboardTransform();
  for (const auto& gizmo : {xGizmo.get(), yGizmo.get(), zGizmo.get()}) {
    if (gizmo->isEnabled()) {
      gizmo->attachedNode = node;
    }
    else {
      gizmo->attachedNode = nullptr;
    }
  };
}

void RotationGizmo::_checkBillboardTransform()
{
  if (_nodeAttached && std::static_pointer_cast<TransformNode>(_nodeAttached)
      && std::static_pointer_cast<TransformNode>(_nodeAttached)->billboardMode()) {
    BABYLON_LOG_ERROR("Rotation Gizmo will not work with transforms in billboard mode.");
  }
}

bool RotationGizmo::get_isHovered() const
{
  auto hovered = false;
  for (const auto& gizmo : {xGizmo.get(), yGizmo.get(), zGizmo.get()}) {
    hovered = hovered || gizmo->isHovered();
  };
  return hovered;
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

void RotationGizmo::addToAxisCache(Mesh* mesh, const GizmoAxisCache& cache)
{
  _gizmoAxisCache[mesh] = cache;
}

void RotationGizmo::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  xGizmo->dispose(doNotRecurse, disposeMaterialAndTextures);
  yGizmo->dispose(doNotRecurse, disposeMaterialAndTextures);
  zGizmo->dispose(doNotRecurse, disposeMaterialAndTextures);
  onDragStartObservable.clear();
  onDragEndObservable.clear();
  for (const auto& obs : _observables) {
    gizmoLayer->utilityLayerScene->onPointerObservable.remove(obs);
  }
}

void RotationGizmo::setCustomMesh(const MeshPtr& /*mesh*/, bool /*useGizmoMaterial*/)
{
  BABYLON_LOG_ERROR(
    "RotationGizmo",
    "Custom meshes are not supported on this gizmo, please set the custom meshes on the gizmos "
    "contained within this one (gizmo.xGizmo, gizmo.yGizmo, gizmo.zGizmo)")
}

} // end of namespace BABYLON
