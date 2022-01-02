#include <babylon/gizmos/axis_scale_gizmo.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/behaviors/meshes/pointer_drag_behavior.h>
#include <babylon/engines/scene.h>
#include <babylon/gizmos/scale_gizmo.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/abstract_mesh.h>
#include <babylon/meshes/builders/box_builder.h>
#include <babylon/meshes/builders/cylinder_builder.h>
#include <babylon/meshes/builders/mesh_builder_options.h>
#include <babylon/meshes/lines_mesh.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

AxisScaleGizmo::AxisScaleGizmo(const Vector3& dragAxis, const Color3& color,
                               const UtilityLayerRendererPtr& iGizmoLayer, ScaleGizmo* parent,
                               float thickness)
    : Gizmo{iGizmoLayer}
    , dragBehavior{std::make_unique<PointerDragBehavior>()}
    , snapDistance{0.f}
    , uniformScaling{false}
    , sensitivity{1.f}
    , isEnabled{this, &AxisScaleGizmo::get_isEnabled, &AxisScaleGizmo::set_isEnabled}
    , _pointerObserver{nullptr}
    , _isEnabled{true}
    , _parent{nullptr}
    , _gizmoMesh{nullptr}
    , _arrowMesh{nullptr}
    , _arrowTail{nullptr}
    , _colliderArrowMesh{nullptr}
    , _colliderArrowTail{nullptr}
    , _coloredMaterial{nullptr}
    , _hoverMaterial{nullptr}
    , _disableMaterial{nullptr}
    , _dragging{false}
    , _currentSnapDragDistance{0.f}
    , _tmpSnapEvent{0.f}
{
  _parent = parent;
  // Create Material
  _coloredMaterial                = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  _coloredMaterial->diffuseColor  = color;
  _coloredMaterial->specularColor = color.subtract(Color3(0.1f, 0.1f, 0.1f));

  _hoverMaterial               = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  _hoverMaterial->diffuseColor = Color3::Yellow();

  _disableMaterial               = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  _disableMaterial->diffuseColor = Color3::Gray();
  _disableMaterial->alpha        = 0.4f;

  // Build mesh + Collider
  _gizmoMesh                       = Mesh::New("axis", gizmoLayer->utilityLayerScene.get());
  std::tie(_arrowMesh, _arrowTail) = _createGizmoMesh(_gizmoMesh, thickness);
  std::tie(_colliderArrowMesh, _colliderArrowTail)
    = _createGizmoMesh(_gizmoMesh, thickness + 4.f, true);

  _gizmoMesh->lookAt(_rootMesh->position().add(dragAxis));
  _rootMesh->addChild(*_gizmoMesh);
  _gizmoMesh->scaling().scaleInPlace(1.f / 3.f);

  // Closure of initial prop values for resetting
  _nodePosition = _arrowMesh->position();
  _linePosition = _arrowTail->position();
  _lineScale    = _arrowTail->scaling();

  _increaseGizmoMesh = [this](float dragDistance) -> void {
    const auto dragStrength = (dragDistance * (3.f / _rootMesh->scaling().length())) * 6.f;

    _arrowMesh->position().z += dragStrength / 3.5f;
    _arrowTail->scaling().y += dragStrength;
    _arrowTail->position().z = _arrowMesh->position().z / 2.f;
  };

  _resetGizmoMesh = [this]() -> void {
    _arrowMesh->position().set(_nodePosition.x, _nodePosition.y, _nodePosition.z);
    _arrowTail->position().set(_linePosition.x, _linePosition.y, _linePosition.z);
    _arrowTail->scaling().set(_lineScale.x, _lineScale.y, _lineScale.z);
    _dragging = false;
  };

  // Add drag behavior to handle events when the gizmo is dragged
  PointerDragBehaviorOptions options;
  options.dragAxis           = dragAxis;
  dragBehavior               = std::make_unique<PointerDragBehavior>(options);
  dragBehavior->moveAttached = false;
  // _rootMesh->addBehavior(dragBehavior);

  dragBehavior->onDragObservable.add([&](DragMoveEvent* event, EventState& /*es*/) {
    if (attachedNode()) {
      // Drag strength is modified by the scale of the gizmo (eg. for small objects like boombox the
      // strength will be increased to match the behavior of larger objects)
      const auto dragStrength
        = sensitivity * event->dragDistance * ((scaleRatio * 3) / _rootMesh->scaling().length());

      // Snapping logic
      auto snapped   = false;
      auto dragSteps = 0;
      if (uniformScaling) {
        std::optional<Vector3> iScale       = _tmpVector;
        std::optional<Quaternion> iRotation = std::nullopt;
        std::optional<Vector3> iTranslation = std::nullopt;
        attachedNode()->getWorldMatrix().decompose(iScale, iRotation, iTranslation);
        _tmpVector = *iScale;
        _tmpVector.normalize();
        if (_tmpVector.y < 0.f) {
          _tmpVector.scaleInPlace(-1.f);
        }
      }
      else {
        _tmpVector.copyFrom(dragAxis);
      }
      if (snapDistance == 0.f) {
        _tmpVector.scaleToRef(dragStrength, _tmpVector);
      }
      else {
        _currentSnapDragDistance += dragStrength;
        if (std::abs(_currentSnapDragDistance) > snapDistance) {
          dragSteps
            = static_cast<int>(std::floor(std::abs(_currentSnapDragDistance) / snapDistance));
          if (_currentSnapDragDistance < 0.f) {
            dragSteps *= -1;
          }
          _currentSnapDragDistance = std::fmod(_currentSnapDragDistance, snapDistance);
          _tmpVector.scaleToRef(snapDistance * dragSteps, _tmpVector);
          snapped = true;
        }
        else {
          _tmpVector.scaleInPlace(0.f);
        }
      }

      Matrix::ScalingToRef(1 + _tmpVector.x, 1 + _tmpVector.y, 1 + _tmpVector.z, _tmpMatrix2);

      _tmpMatrix2.multiplyToRef(attachedNode()->getWorldMatrix(), _tmpMatrix);
      std::optional<Vector3> iScale       = __tmpVector;
      std::optional<Quaternion> iRotation = std::nullopt;
      std::optional<Vector3> iTranslation = std::nullopt;
      _tmpMatrix.decompose(iScale, iRotation, iTranslation);
      __tmpVector = *iScale;

      const auto maxScale = 100000.f;
      if (std::abs(_tmpVector.x) < maxScale && std::abs(_tmpVector.y) < maxScale
          && std::abs(_tmpVector.z) < maxScale) {
        attachedNode()->getWorldMatrix().copyFrom(_tmpMatrix);
      }

      if (snapped) {
        _tmpSnapEvent.snapDistance = snapDistance * dragSteps;
        onSnapObservable.notifyObservers(&_tmpSnapEvent);
      }
      _matrixChanged();
    }
  });

  // Selection/deselection
  dragBehavior->onDragStartObservable.add(
    [&](DragStartOrEndEvent* /*event*/, EventState& /*es*/) -> void { _dragging = true; });
  dragBehavior->onDragObservable.add(
    [this](DragMoveEvent* e, EventState& /*es*/) -> void { _increaseGizmoMesh(e->dragDistance); });
  dragBehavior->onDragEndObservable.add(
    [this](DragStartOrEndEvent* /*event*/, EventState& /*es*/) -> void { _resetGizmoMesh(); });

  _cache.gizmoMeshes     = {_arrowMesh, _arrowTail};
  _cache.colliderMeshes  = {_colliderArrowMesh, _colliderArrowTail};
  _cache.material        = _coloredMaterial;
  _cache.hoverMaterial   = _hoverMaterial;
  _cache.disableMaterial = _disableMaterial;
  _cache.active          = false;
  if (_parent) {
    _parent->addToAxisCache(static_cast<Mesh*>(_gizmoMesh.get()), _cache);
  }

  _pointerObserver = gizmoLayer->utilityLayerScene->onPointerObservable.add(
    [&](PointerInfo* pointerInfo, EventState& /*es*/) {
      if (_customMeshSet) {
        return;
      }

      auto pickedMesh = pointerInfo->pickInfo.pickedMesh;
      auto it = std::find(_rootMesh->getChildMeshes().begin(), _rootMesh->getChildMeshes().end(),
                          pickedMesh);

      _isHovered    = (it != _rootMesh->getChildMeshes().end());
      auto material = _isHovered || _dragging ? _hoverMaterial : _coloredMaterial;
      for (auto& m : _cache.gizmoMeshes) {
        m->material    = material;
        auto linesMesh = std::static_pointer_cast<LinesMesh>(m);
        if (linesMesh) {
          linesMesh->color = material->diffuseColor;
        }
      }
    });

  const auto& light = gizmoLayer->_getSharedGizmoLight();
  light->includedOnlyMeshes
    = stl_util::concat(light->includedOnlyMeshes(), _rootMesh->getChildMeshes());
}

AxisScaleGizmo::~AxisScaleGizmo() = default;

std::tuple<MeshPtr, MeshPtr> AxisScaleGizmo::_createGizmoMesh(const AbstractMeshPtr& parentMesh,
                                                              float thickness, bool isCollider)
{
  BoxOptions boxOptions;
  boxOptions.size = 0.4f * (1.f + (thickness - 1.f) / 4.f);
  auto arrowMesh
    = BoxBuilder::CreateBox("yPosMesh", boxOptions, gizmoLayer->utilityLayerScene.get());

  CylinderOptions cylinderOptions;
  cylinderOptions.diameterTop    = 0.005f * thickness;
  cylinderOptions.height         = 0.275f;
  cylinderOptions.diameterBottom = 0.005f * thickness;
  cylinderOptions.tessellation   = 96;
  auto arrowTail                 = CylinderBuilder::CreateCylinder("cylinder", cylinderOptions,
                                                   gizmoLayer->utilityLayerScene.get());
  arrowTail->material            = _coloredMaterial;

  // Position arrow pointing in its drag axis
  arrowMesh->scaling().scaleInPlace(0.1f);
  arrowMesh->material     = _coloredMaterial;
  arrowMesh->rotation().x = Math::PI / 2.f;
  arrowMesh->position().z += 0.3f;

  arrowTail->material = _coloredMaterial;
  arrowTail->position().z += 0.275f / 2.f;
  arrowTail->rotation().x = Math::PI / 2;

  if (isCollider) {
    arrowMesh->visibility = 0.f;
    arrowTail->visibility = 0.f;
  }

  parentMesh->addChild(*arrowMesh);
  parentMesh->addChild(*arrowTail);

  return {arrowMesh, arrowTail};
}

void AxisScaleGizmo::_attachedNodeChanged(const NodePtr& value)
{
  if (dragBehavior) {
    dragBehavior->enabled = value ? true : false;
  }
}

void AxisScaleGizmo::set_isEnabled(bool value)
{
  _isEnabled = value;
  if (!value) {
    attachedMesh = nullptr;
    attachedNode = nullptr;
  }
  else {
    if (_parent) {
      attachedMesh = _parent->attachedMesh();
      attachedNode = _parent->attachedNode();
    }
  }
}

bool AxisScaleGizmo::get_isEnabled() const
{
  return _isEnabled;
}

void AxisScaleGizmo::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  onSnapObservable.clear();
  gizmoLayer->utilityLayerScene->onPointerObservable.remove(_pointerObserver);
  dragBehavior->detach();
  if (_gizmoMesh) {
    _gizmoMesh->dispose();
  }
  for (const auto& matl : {_coloredMaterial, _hoverMaterial, _disableMaterial}) {
    if (matl) {
      matl->dispose();
    }
  }
  Gizmo::dispose(doNotRecurse, disposeMaterialAndTextures);
}

void AxisScaleGizmo::setCustomMesh(const MeshPtr& mesh, bool useGizmoMaterial)
{
  Gizmo::setCustomMesh(mesh);
  if (useGizmoMaterial) {
    for (const auto& m : _rootMesh->getChildMeshes()) {
      m->material = _coloredMaterial;
      if (auto lm = std::static_pointer_cast<LinesMesh>(m)) {
        lm->color = _coloredMaterial->diffuseColor;
      }
    }
    _customMeshSet = false;
  }
}

} // end of namespace BABYLON
