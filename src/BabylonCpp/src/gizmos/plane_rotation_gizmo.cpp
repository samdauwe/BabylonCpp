#include <babylon/gizmos/plane_rotation_gizmo.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/behaviors/meshes/pointer_drag_behavior.h>
#include <babylon/cameras/camera.h>
#include <babylon/engines/scene.h>
#include <babylon/gizmos/rotation_gizmo.h>
#include <babylon/gizmos/scale_gizmo.h>
#include <babylon/lights/hemispheric_light.h>
#include <babylon/materials/standard_material.h>
#include <babylon/meshes/lines_mesh.h>
#include <babylon/meshes/mesh.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

PlaneRotationGizmo::PlaneRotationGizmo(
  const Vector3& planeNormal, const Color3& color,
  const std::shared_ptr<UtilityLayerRenderer>& iGizmoLayer,
  unsigned int tessellation, RotationGizmo* parent, bool useEulerRotation)
    : Gizmo{iGizmoLayer}
    , dragBehavior{nullptr}
    , snapDistance{0.f}
    , isEnabled{this, &PlaneRotationGizmo::get_isEnabled,
                &PlaneRotationGizmo::set_isEnabled}
    , _pointerObserver{nullptr}
    , _isEnabled{true}
    , _parent{nullptr}
    , _tmpSnapEvent{0.f}
{
  _parent = parent;
  // Create Material
  auto coloredMaterial
    = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  coloredMaterial->diffuseColor  = color;
  coloredMaterial->specularColor = color.subtract(Color3(0.1f, 0.1f, 0.1f));

  auto hoverMaterial
    = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  hoverMaterial->diffuseColor = color.add(Color3(0.3f, 0.3f, 0.3f));

  // Build mesh on root node
  auto parentMesh = AbstractMesh::New("", gizmoLayer->utilityLayerScene.get());

  auto drag              = Mesh::CreateTorus("", 0.6f, 0.03f, tessellation,
                                gizmoLayer->utilityLayerScene.get());
  drag->visibility       = 0.f;
  auto rotationMesh      = Mesh::CreateTorus("", 0.6f, 0.005f, tessellation,
                                        gizmoLayer->utilityLayerScene.get());
  rotationMesh->material = coloredMaterial;

  // Position arrow pointing in its drag axis
  rotationMesh->rotation().x = Math::PI_2;
  drag->rotation().x         = Math::PI_2;
  parentMesh->addChild(*rotationMesh);
  parentMesh->addChild(*drag);
  parentMesh->lookAt(_rootMesh->position().add(planeNormal));

  _rootMesh->addChild(*parentMesh);
  parentMesh->scaling().scaleInPlace(1.f / 3.f);
  // Add drag behavior to handle events when the gizmo is dragged
  PointerDragBehaviorOptions options;
  options.dragPlaneNormal    = planeNormal;
  dragBehavior               = std::make_unique<PointerDragBehavior>(options);
  dragBehavior->moveAttached = false;
  dragBehavior->maxDragAngle = Math::PI * 9.f / 20.f;
  dragBehavior->_useAlternatePickedPointAboveMaxDragAngle = true;
  // _rootMesh->addBehavior(dragBehavior.get());

  dragBehavior->onDragStartObservable.add(
    [&](DragStartOrEndEvent* e, EventState& /*es*/) {
      if (attachedMesh()) {
        _lastDragPosition.copyFrom(e->dragPlanePoint);
      }
    });

  _currentSnapDragDistance = 0.f;
  dragBehavior->onDragObservable.add([&](DragMoveEvent* event,
                                         EventState& /*es*/) {
    if (attachedMesh()) {
      if (!attachedMesh()->rotationQuaternion() || useEulerRotation) {
        attachedMesh()->rotationQuaternion = Quaternion::RotationYawPitchRoll(
          attachedMesh()->rotation().y, attachedMesh()->rotation().x,
          attachedMesh()->rotation().z);
      }

      // Remove parent priort to rotating
      auto attachedMeshParent = attachedMesh()->parent();
      if (attachedMeshParent) {
        attachedMesh()->setParent(nullptr);
      }

      // Calc angle over full 360 degree
      // (https://stackoverflow.com/questions/43493711/the-angle-between-two-3d-vectors-with-a-result-range-0-360)
      auto Vector
        = event->dragPlanePoint.subtract(attachedMesh()->absolutePosition)
            .normalize();
      auto originalVector
        = _lastDragPosition.subtract(attachedMesh()->absolutePosition)
            .normalize();
      auto cross = Vector3::Cross(Vector, originalVector);
      auto dot   = Vector3::Dot(Vector, originalVector);
      auto angle = std::atan2(cross.length(), dot);
      _planeNormalTowardsCamera.copyFrom(planeNormal);
      _localPlaneNormalTowardsCamera.copyFrom(planeNormal);
      if (updateGizmoRotationToMatchAttachedMesh) {
        attachedMesh()->rotationQuaternion()->toRotationMatrix(_rotationMatrix);
        _localPlaneNormalTowardsCamera = Vector3::TransformCoordinates(
          _planeNormalTowardsCamera, _rotationMatrix);
      }
      // Flip up vector depending on which side the camera is on
      if (gizmoLayer->utilityLayerScene.get()->activeCamera()) {
        auto camVec = gizmoLayer->utilityLayerScene.get()
                        ->activeCamera()
                        ->position()
                        .subtract(attachedMesh()->position());
        if (Vector3::Dot(camVec, _localPlaneNormalTowardsCamera) > 0.f) {
          _planeNormalTowardsCamera.scaleInPlace(-1.f);
          _localPlaneNormalTowardsCamera.scaleInPlace(-1.f);
        }
      }
      auto halfCircleSide
        = Vector3::Dot(_localPlaneNormalTowardsCamera, cross) > 0.f;
      if (halfCircleSide) {
        angle = -angle;
      }

      // Snapping logic
      auto snapped = false;
      if (snapDistance != 0.f) {
        _currentSnapDragDistance += angle;
        if (std::abs(_currentSnapDragDistance) > snapDistance) {
          auto dragSteps
            = std::floor(std::abs(_currentSnapDragDistance) / snapDistance);
          if (_currentSnapDragDistance < 0.f) {
            dragSteps *= -1.f;
          }
          _currentSnapDragDistance
            = std::fmod(_currentSnapDragDistance, snapDistance);
          angle   = snapDistance * dragSteps;
          snapped = true;
        }
        else {
          angle = 0;
        }
      }

      // If the mesh has a parent, convert needed world rotation to local
      // rotation
      _tmpMatrix.reset();
      if (attachedMesh()->parent) {
        attachedMesh()->parent()->computeWorldMatrix().invertToRef(_tmpMatrix);
        _tmpMatrix.getRotationMatrixToRef(_tmpMatrix);
        Vector3::TransformCoordinatesToRef(
          _planeNormalTowardsCamera, _tmpMatrix, _planeNormalTowardsCamera);
      }

      // Convert angle and axis to quaternion
      // (http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToQuaternion/index.htm)
      auto quaternionCoefficient = std::sin(angle / 2.f);
      _amountToRotate.set(_planeNormalTowardsCamera.x * quaternionCoefficient,
                          _planeNormalTowardsCamera.y * quaternionCoefficient,
                          _planeNormalTowardsCamera.z * quaternionCoefficient,
                          std::cos(angle / 2.f));

      // If the meshes local scale is inverted (eg. loaded gltf file parent with
      // z scale of -1) the rotation needs to be inverted on the y axis
      if (_tmpMatrix.determinant() > 0) {
        _amountToRotate.toEulerAnglesToRef(_tmpVector);
        Quaternion::RotationYawPitchRollToRef(_tmpVector.y, -_tmpVector.x,
                                              -_tmpVector.z, _amountToRotate);
      }

      if (updateGizmoRotationToMatchAttachedMesh) {
        // Rotate selected mesh quaternion over fixed axis
        attachedMesh()->rotationQuaternion()->multiplyToRef(
          _amountToRotate, *attachedMesh()->rotationQuaternion());
      }
      else {
        // Rotate selected mesh quaternion over rotated axis
        _amountToRotate.multiplyToRef(*attachedMesh()->rotationQuaternion(),
                                      *attachedMesh()->rotationQuaternion());
      }

      if (useEulerRotation) {
        attachedMesh()->rotationQuaternion()->toEulerAnglesToRef(_tmpVector);
        attachedMesh()->rotationQuaternion = std::nullopt;
        attachedMesh()->rotation().copyFrom(_tmpVector);
      }

      _lastDragPosition.copyFrom(event->dragPlanePoint);
      if (snapped) {
        _tmpSnapEvent.snapDistance = angle;
        onSnapObservable.notifyObservers(&_tmpSnapEvent);
      }

      // Restore parent
      if (attachedMeshParent) {
        attachedMesh()->setParent(attachedMeshParent);
      }
    }
  });

  _pointerObserver = gizmoLayer->utilityLayerScene->onPointerObservable.add(
    [&](PointerInfo* pointerInfo, EventState& /*es*/) {
      if (_customMeshSet) {
        return;
      }
      auto isHovered = stl_util::contains(_rootMesh->getChildMeshes(),
                                          pointerInfo->pickInfo.pickedMesh);
      auto material  = isHovered ? hoverMaterial : coloredMaterial;
      for (auto& m : _rootMesh->getChildMeshes()) {
        m->material = material;
        // if ((static_cast<LinesMesh*>(m))->color)
        {
          std::static_pointer_cast<LinesMesh>(m)->color
            = material->diffuseColor;
        }
      }
    });

  const auto& light         = gizmoLayer->_getSharedGizmoLight();
  light->includedOnlyMeshes = stl_util::concat(
    light->includedOnlyMeshes(), _rootMesh->getChildMeshes(false));
}

PlaneRotationGizmo::~PlaneRotationGizmo()
{
}

void PlaneRotationGizmo::_attachedMeshChanged(const AbstractMeshPtr& value)
{
  if (dragBehavior) {
    dragBehavior->enabled = value ? true : false;
  }
}

void PlaneRotationGizmo::set_isEnabled(bool value)
{
  _isEnabled = value;
  if (!value) {
    attachedMesh = nullptr;
  }
  else {
    if (_parent) {
      attachedMesh = _parent->attachedMesh();
    }
  }
}

bool PlaneRotationGizmo::get_isEnabled() const
{
  return _isEnabled;
}

void PlaneRotationGizmo::dispose(bool doNotRecurse,
                                 bool disposeMaterialAndTextures)
{
  onSnapObservable.clear();
  gizmoLayer->utilityLayerScene->onPointerObservable.remove(_pointerObserver);
  dragBehavior->detach();
  Gizmo::dispose(doNotRecurse, disposeMaterialAndTextures);
}

} // end of namespace BABYLON
