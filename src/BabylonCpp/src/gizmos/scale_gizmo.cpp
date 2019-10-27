#include <babylon/gizmos/scale_gizmo.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/behaviors/meshes/pointer_drag_behavior.h>
#include <babylon/core/logging.h>
#include <babylon/gizmos/axis_scale_gizmo.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/math/color3.h>
#include <babylon/math/vector3.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/builders/polyhedron_builder.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {

ScaleGizmo::ScaleGizmo(const std::shared_ptr<UtilityLayerRenderer>& iGizmoLayer)
    : Gizmo{iGizmoLayer}
    , snapDistance{this, &ScaleGizmo::get_snapDistance,
                   &ScaleGizmo::set_snapDistance}
    , _meshAttached{nullptr}
    , _updateGizmoRotationToMatchAttachedMesh{false}
    , _snapDistance{0.f}
    , _scaleRatio{1.f}
    , _uniformScalingMesh{nullptr}
    , _octahedron{nullptr}
{
  xGizmo = std::make_unique<AxisScaleGizmo>(
    Vector3(1.f, 0.f, 0.f), Color3::Red().scale(0.5f), iGizmoLayer, this);
  yGizmo = std::make_unique<AxisScaleGizmo>(
    Vector3(0.f, 1.f, 0.f), Color3::Green().scale(0.5f), iGizmoLayer, this);
  zGizmo = std::make_unique<AxisScaleGizmo>(
    Vector3(0.f, 0.f, 1.f), Color3::Blue().scale(0.5f), iGizmoLayer, this);

  // Create uniform scale gizmo
  uniformScaleGizmo = std::make_unique<AxisScaleGizmo>(
    Vector3(0.f, 1.f, 0.f), Color3::Yellow().scale(0.5f), iGizmoLayer, this);
  uniformScaleGizmo->updateGizmoRotationToMatchAttachedMesh = false;
  uniformScaleGizmo->uniformScaling                         = true;
  PolyhedronOptions options;
  options.type        = 1;
  _uniformScalingMesh = PolyhedronBuilder::CreatePolyhedron(
    "", options, uniformScaleGizmo->gizmoLayer->utilityLayerScene.get());
  _uniformScalingMesh->scaling().scaleInPlace(0.02f);
  _uniformScalingMesh->visibility = 0.f;
  _octahedron                     = PolyhedronBuilder::CreatePolyhedron(
    "", options, uniformScaleGizmo->gizmoLayer->utilityLayerScene.get());
  _octahedron->scaling().scaleInPlace(0.007f);
  _uniformScalingMesh->addChild(*_octahedron);
  uniformScaleGizmo->setCustomMesh(_uniformScalingMesh, true);
  const auto& light = gizmoLayer->_getSharedGizmoLight();
  light->includedOnlyMeshes
    = stl_util::concat(light->includedOnlyMeshes(), {_octahedron});

  // Relay drag events
  for (const auto& gizmo :
       {xGizmo.get(), yGizmo.get(), zGizmo.get(), uniformScaleGizmo.get()}) {
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

ScaleGizmo::~ScaleGizmo()
{
}

AbstractMeshPtr& ScaleGizmo::get_attachedMesh()
{
  return _meshAttached;
}

void ScaleGizmo::set_attachedMesh(const AbstractMeshPtr& mesh)
{
  _meshAttached = mesh;
  for (const auto& gizmo :
       {xGizmo.get(), yGizmo.get(), zGizmo.get(), uniformScaleGizmo.get()}) {
    if (gizmo->isEnabled()) {
      gizmo->attachedMesh = mesh;
    }
    else {
      gizmo->attachedMesh = nullptr;
    }
  }
}

void ScaleGizmo::set_updateGizmoRotationToMatchAttachedMesh(bool value)
{
  if (!value) {
    BABYLON_LOG_WARN("ScaleGizmo",
                     "Setting updateGizmoRotationToMatchAttachedMesh = false "
                     "on scaling gizmo is not supported.")
  }
  else {
    _updateGizmoRotationToMatchAttachedMesh = value;
    for (const auto& gizmo :
         {xGizmo.get(), yGizmo.get(), zGizmo.get(), uniformScaleGizmo.get()}) {
      if (gizmo) {
        gizmo->updateGizmoRotationToMatchAttachedMesh = value;
      }
    }
  }
}

bool ScaleGizmo::get_updateGizmoRotationToMatchAttachedMesh() const
{
  return _updateGizmoRotationToMatchAttachedMesh;
}

void ScaleGizmo::set_snapDistance(float value)
{
  _snapDistance = value;
  for (const auto& gizmo :
       {xGizmo.get(), yGizmo.get(), zGizmo.get(), uniformScaleGizmo.get()}) {
    if (gizmo) {
      gizmo->snapDistance = value;
    }
  }
}

float ScaleGizmo::get_snapDistance() const
{
  return _snapDistance;
}

void ScaleGizmo::set_scaleRatio(float value)
{
  _scaleRatio = value;
  for (const auto& gizmo :
       {xGizmo.get(), yGizmo.get(), zGizmo.get(), uniformScaleGizmo.get()}) {
    if (gizmo) {
      gizmo->scaleRatio = value;
    }
  }
}

float ScaleGizmo::get_scaleRatio() const
{
  return _scaleRatio;
}

void ScaleGizmo::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  for (const auto& gizmo :
       {xGizmo.get(), yGizmo.get(), zGizmo.get(), uniformScaleGizmo.get()}) {
    if (gizmo) {
      gizmo->dispose(doNotRecurse, disposeMaterialAndTextures);
    }
  }
  onDragStartObservable.clear();
  onDragEndObservable.clear();

  for (const auto& msh : {_uniformScalingMesh, _octahedron}) {
    if (msh) {
      msh->dispose(doNotRecurse, disposeMaterialAndTextures);
    }
  }
} // namespace BABYLON

} // end of namespace BABYLON
