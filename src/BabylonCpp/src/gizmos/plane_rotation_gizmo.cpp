#include <babylon/gizmos/plane_rotation_gizmo.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/behaviors/mesh/pointer_drag_behavior.h>
#include <babylon/cameras/camera.h>
#include <babylon/engine/scene.h>
#include <babylon/materials/standard_material.h>
#include <babylon/mesh/lines_mesh.h>
#include <babylon/mesh/mesh.h>
#include <babylon/rendering/utility_layer_renderer.h>

namespace BABYLON {

PlaneRotationGizmo::PlaneRotationGizmo(
  const Vector3& planeNormal, const Color3& color,
  const shared_ptr_t<UtilityLayerRenderer>& iGizmoLayer)
    : Gizmo{iGizmoLayer}
    , snapDistance{0.f}
    , _pointerObserver{nullptr}
    , _tmpSnapEvent{0.f}
{
  // Create Material
  auto coloredMaterial
    = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  coloredMaterial->setDisableLighting(true);
  coloredMaterial->emissiveColor = color;

  auto hoverMaterial
    = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  hoverMaterial->setDisableLighting(true);
  hoverMaterial->emissiveColor = color.add(Color3(0.2f, 0.2f, 0.2f));

  // Build mesh on root node
  auto parentMesh = AbstractMesh::New("", gizmoLayer->utilityLayerScene.get());

  // Create circle out of lines
  auto tessellation = 20ul;
  auto radius       = 2.f;
  vector_t<Vector3> points;
  for (size_t i = 0; i < tessellation; ++i) {
    auto radian = (Math::PI2) * (i / (tessellation - 1));
    points.emplace_back(
      Vector3(radius * ::std::sin(radian), 0.f, radius * ::std::cos(radian)));
  }
  auto rotationMesh
    = Mesh::CreateLines("", points, gizmoLayer->utilityLayerScene.get());
  rotationMesh->color = coloredMaterial->emissiveColor;

  // Position arrow pointing in its drag axis
  rotationMesh->scaling().scaleInPlace(0.1f);
  rotationMesh->material     = coloredMaterial;
  rotationMesh->rotation().x = Math::PI_2;
  parentMesh->addChild(*rotationMesh);
  parentMesh->lookAt(_rootMesh->position().subtract(planeNormal));

  _rootMesh->addChild(*parentMesh);
  // Add drag behavior to handle events when the gizmo is dragged
  PointerDragBehaviorOptions options;
  options.dragPlaneNormal    = planeNormal;
  dragBehavior               = ::std::make_unique<PointerDragBehavior>(options);
  dragBehavior->moveAttached = false;
  dragBehavior->maxDragAngle = Math::PI * 9.f / 20.f;
  dragBehavior->_useAlternatePickedPointAboveMaxDragAngle = true;
  _rootMesh->addBehavior(dragBehavior.get());

  dragBehavior->onDragStartObservable.add(
    [&](DragStartOrEndEvent* e, EventState& /*es*/) {
      if (attachedMesh) {
        _lastDragPosition.copyFrom(e->dragPlanePoint);
      }
    });

  auto currentSnapDragDistance = 0.f;
  dragBehavior->onDragObservable.add([&](DragMoveEvent* event,
                                         EventState& /*es*/) {
    if (attachedMesh) {
      if (!attachedMesh()->rotationQuaternion()) {
        attachedMesh()->rotationQuaternion = Quaternion::RotationYawPitchRoll(
          attachedMesh()->rotation().y, attachedMesh()->rotation().x,
          attachedMesh()->rotation().z);
      }
      // Calc angle over full 360 degree
      // (https://stackoverflow.com/questions/43493711/the-angle-between-two-3d-vectors-with-a-result-range-0-360)
      auto newVector
        = event->dragPlanePoint.subtract(attachedMesh()->position())
            .normalize();
      auto originalVector
        = _lastDragPosition.subtract(attachedMesh()->position()).normalize();
      auto cross = Vector3::Cross(newVector, originalVector);
      auto dot   = Vector3::Dot(newVector, originalVector);
      auto angle = ::std::atan2(cross.length(), dot);
      _planeNormalTowardsCamera.copyFrom(planeNormal);
      _localPlaneNormalTowardsCamera.copyFrom(planeNormal);
      if (updateGizmoRotationToMatchAttachedMesh) {
        attachedMesh()->rotationQuaternion()->toRotationMatrix(_rotationMatrix);
        _localPlaneNormalTowardsCamera = Vector3::TransformCoordinates(
          _planeNormalTowardsCamera, _rotationMatrix);
      }
      // Flip up vector depending on which side the camera is on
      if (gizmoLayer->utilityLayerScene.get()->activeCamera) {
        auto camVec
          = gizmoLayer->utilityLayerScene.get()
              ->activeCamera->position.subtract(attachedMesh()->position());
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
        currentSnapDragDistance += angle;
        if (::std::abs(currentSnapDragDistance) > snapDistance) {
          auto dragSteps = ::std::floor(currentSnapDragDistance / snapDistance);
          currentSnapDragDistance
            = ::std::fmod(currentSnapDragDistance, snapDistance);
          angle   = snapDistance * dragSteps;
          snapped = true;
        }
        else {
          angle = 0;
        }
      }
      // Convert angle and axis to quaternion
      // (http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToQuaternion/index.htm)
      auto quaternionCoefficient = ::std::sin(angle / 2.f);
      auto amountToRotate
        = Quaternion(_planeNormalTowardsCamera.x * quaternionCoefficient,
                     _planeNormalTowardsCamera.y * quaternionCoefficient,
                     _planeNormalTowardsCamera.z * quaternionCoefficient,
                     ::std::cos(angle / 2.f));

      if (updateGizmoRotationToMatchAttachedMesh) {
        // Rotate selected mesh quaternion over fixed axis
        attachedMesh()->rotationQuaternion()->multiplyToRef(
          amountToRotate, *attachedMesh()->rotationQuaternion());
      }
      else {
        // Rotate selected mesh quaternion over rotated axis
        amountToRotate.multiplyToRef(*attachedMesh()->rotationQuaternion(),
                                     *attachedMesh()->rotationQuaternion());
      }

      _lastDragPosition.copyFrom(event->dragPlanePoint);
      if (snapped) {
        _tmpSnapEvent.snapDistance = angle;
        onSnapObservable.notifyObservers(&_tmpSnapEvent);
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
          ::std::static_pointer_cast<LinesMesh>(m)->color
            = material->emissiveColor;
        }
      };
    });
}

PlaneRotationGizmo::~PlaneRotationGizmo()
{
}

void PlaneRotationGizmo::_attachedMeshChanged(AbstractMesh* value)
{
  if (dragBehavior) {
    dragBehavior->enabled = value ? true : false;
  }
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
