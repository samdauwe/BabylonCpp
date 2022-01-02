#include <babylon/gizmos/scale_gizmo.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/behaviors/meshes/pointer_drag_behavior.h>
#include <babylon/core/logging.h>
#include <babylon/engines/scene.h>
#include <babylon/gizmos/axis_scale_gizmo.h>
#include <babylon/gizmos/bounding_box_gizmo.h>
#include <babylon/gizmos/gizmo_manager.h>
#include <babylon/gizmos/position_gizmo.h>
#include <babylon/gizmos/rotation_gizmo.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/maths/color3.h>
#include <babylon/maths/vector3.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/builders/polyhedron_builder.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {

ScaleGizmo::ScaleGizmo(const std::shared_ptr<UtilityLayerRenderer>& iGizmoLayer, float thickness,
                       GizmoManager* gizmoManager)
    : Gizmo{iGizmoLayer}
    , snapDistance{this, &ScaleGizmo::get_snapDistance, &ScaleGizmo::set_snapDistance}
    , sensitivity{this, &ScaleGizmo::get_sensitivity, &ScaleGizmo::set_sensitivity}
    , _snapDistance{0.f}
    , _uniformScalingMesh{nullptr}
    , _octahedron{nullptr}
    , _sensitivity{1.f}
{
  uniformScaleGizmo = _createUniformScaleMesh();
  xGizmo = std::make_unique<AxisScaleGizmo>(Vector3(1.f, 0.f, 0.f), Color3::Red().scale(0.5f),
                                            iGizmoLayer, this, thickness);
  yGizmo = std::make_unique<AxisScaleGizmo>(Vector3(0.f, 1.f, 0.f), Color3::Green().scale(0.5f),
                                            iGizmoLayer, this, thickness);
  zGizmo = std::make_unique<AxisScaleGizmo>(Vector3(0.f, 0.f, 1.f), Color3::Blue().scale(0.5f),
                                            iGizmoLayer, this, thickness);

  // Relay drag events
  for (const auto& gizmo : {xGizmo.get(), yGizmo.get(), zGizmo.get(), uniformScaleGizmo.get()}) {
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

ScaleGizmo::~ScaleGizmo() = default;

AbstractMeshPtr& ScaleGizmo::get_attachedMesh()
{
  return _meshAttached;
}

void ScaleGizmo::set_attachedMesh(const AbstractMeshPtr& mesh)
{
  _meshAttached = mesh;
  _nodeAttached = mesh;
  for (const auto& gizmo : {xGizmo.get(), yGizmo.get(), zGizmo.get(), uniformScaleGizmo.get()}) {
    if (gizmo->isEnabled()) {
      gizmo->attachedMesh = mesh;
    }
    else {
      gizmo->attachedMesh = nullptr;
    }
  }
}

NodePtr& ScaleGizmo::get_attachedNode()
{
  return _nodeAttached;
}

void ScaleGizmo::set_attachedNode(const NodePtr& node)
{
  _meshAttached = nullptr;
  _nodeAttached = node;
  for (const auto& gizmo : {xGizmo.get(), yGizmo.get(), zGizmo.get(), uniformScaleGizmo.get()}) {
    if (gizmo->isEnabled()) {
      gizmo->attachedNode = node;
    }
    else {
      gizmo->attachedNode = nullptr;
    }
  }
}

bool ScaleGizmo::get_isHovered() const
{
  auto hovered = false;
  for (const auto& gizmo : {xGizmo.get(), yGizmo.get(), zGizmo.get()}) {
    hovered = hovered || gizmo->isHovered();
  }
  return hovered;
}

std::unique_ptr<AxisScaleGizmo> ScaleGizmo::_createUniformScaleMesh()
{
  _coloredMaterial               = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  _coloredMaterial->diffuseColor = Color3::Gray();

  _hoverMaterial               = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  _hoverMaterial->diffuseColor = Color3::Yellow();

  _disableMaterial               = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  _disableMaterial->diffuseColor = Color3::Gray();
  _disableMaterial->alpha        = 0.4f;

  auto iUniformScaleGizmo = std::make_unique<AxisScaleGizmo>(
    Vector3(0.f, 1.f, 0.f), Color3::Gray().scale(0.5f), gizmoLayer, this);
  iUniformScaleGizmo->updateGizmoRotationToMatchAttachedMesh = false;
  iUniformScaleGizmo->uniformScaling                         = true;
  PolyhedronOptions options;
  options.type        = 1;
  _uniformScalingMesh = PolyhedronBuilder::CreatePolyhedron(
    "uniform", options, iUniformScaleGizmo->gizmoLayer->utilityLayerScene.get());
  _uniformScalingMesh->scaling().scaleInPlace(0.01f);
  _uniformScalingMesh->visibility = 0.f;
  _octahedron                     = PolyhedronBuilder::CreatePolyhedron(
    "", options, iUniformScaleGizmo->gizmoLayer->utilityLayerScene.get());
  _octahedron->scaling().scaleInPlace(0.007f);
  _uniformScalingMesh->addChild(*_octahedron);
  iUniformScaleGizmo->setCustomMesh(_uniformScalingMesh, true);
  const auto& light         = gizmoLayer->_getSharedGizmoLight();
  light->includedOnlyMeshes = stl_util::concat(light->includedOnlyMeshes(), {_octahedron});

  GizmoAxisCache cache;
  cache.gizmoMeshes     = {_octahedron, _uniformScalingMesh};
  cache.colliderMeshes  = {_uniformScalingMesh};
  cache.material        = _coloredMaterial;
  cache.hoverMaterial   = _hoverMaterial;
  cache.disableMaterial = _disableMaterial;
  cache.active          = false;

  addToAxisCache(iUniformScaleGizmo->_rootMesh.get(), cache);

  return iUniformScaleGizmo;
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
    for (const auto& gizmo : {xGizmo.get(), yGizmo.get(), zGizmo.get(), uniformScaleGizmo.get()}) {
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
  for (const auto& gizmo : {xGizmo.get(), yGizmo.get(), zGizmo.get(), uniformScaleGizmo.get()}) {
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
  for (const auto& gizmo : {xGizmo.get(), yGizmo.get(), zGizmo.get(), uniformScaleGizmo.get()}) {
    if (gizmo) {
      gizmo->scaleRatio = value;
    }
  }
}

float ScaleGizmo::get_scaleRatio() const
{
  return _scaleRatio;
}

void ScaleGizmo::set_sensitivity(float value)
{
  _sensitivity = value;
  for (const auto& gizmo : {xGizmo.get(), yGizmo.get(), zGizmo.get(), uniformScaleGizmo.get()}) {
    if (gizmo) {
      gizmo->sensitivity = value;
    }
  }
}

float ScaleGizmo::get_sensitivity() const
{
  return _sensitivity;
}

void ScaleGizmo::addToAxisCache(Mesh* mesh, const GizmoAxisCache& cache)
{
  _gizmoAxisCache[mesh] = cache;
}

void ScaleGizmo::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  for (const auto& gizmo : {xGizmo.get(), yGizmo.get(), zGizmo.get(), uniformScaleGizmo.get()}) {
    if (gizmo) {
      gizmo->dispose(doNotRecurse, disposeMaterialAndTextures);
    }
  }
  for (const auto& obs : _observables) {
    gizmoLayer->utilityLayerScene->onPointerObservable.remove(obs);
  }
  onDragStartObservable.clear();
  onDragEndObservable.clear();

  for (const auto& msh : {_uniformScalingMesh, _octahedron}) {
    if (msh) {
      msh->dispose(doNotRecurse, disposeMaterialAndTextures);
    }
  }
  for (const auto& matl : {_coloredMaterial, _hoverMaterial, _disableMaterial}) {
    if (matl) {
      matl->dispose();
    }
  }
}

} // end of namespace BABYLON
