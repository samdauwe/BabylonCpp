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

float PlaneRotationGizmo::MaxDragAngle = Math::PI * 9.f / 20.f;

PlaneRotationGizmo::CircleConstants PlaneRotationGizmo::_CircleConstants = {
  0.3f,           // radius
  Math::PI * 2.f, // pi2
  70u,            // tessellation
  4,              // rotationCircleRange
};

PlaneRotationGizmo::PlaneRotationGizmo(const Vector3& planeNormal, const Color3& color,
                                       const std::shared_ptr<UtilityLayerRenderer>& iGizmoLayer,
                                       unsigned int tessellation, RotationGizmo* parent,
                                       bool useEulerRotation, float thickness)
    : Gizmo{iGizmoLayer}
    , dragBehavior{nullptr}
    , snapDistance{0.f}
    , angle{0.f}
    , isEnabled{this, &PlaneRotationGizmo::get_isEnabled, &PlaneRotationGizmo::set_isEnabled}
    , _pointerObserver{nullptr}
    , _isEnabled{true}
    , _parent{nullptr}
    , _coloredMaterial{nullptr}
    , _hoverMaterial{nullptr}
    , _disableMaterial{nullptr}
    , _gizmoMesh{nullptr}
    , _rotationCircle{nullptr}
    , _dragging{false}
    , _useEulerRotation{useEulerRotation}
    , _dragDistance{0.f}
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

  // Build mesh on root node
  _gizmoMesh                         = Mesh::New("", gizmoLayer->utilityLayerScene.get());
  std::tie(_rotationMesh, _collider) = _createGizmoMesh(_gizmoMesh, thickness, tessellation);

  // Setup Rotation Circle
  std::vector<std::vector<Vector3>> rotationCirclePaths;
  _rotationCircle = setupRotationCircle(rotationCirclePaths, _gizmoMesh);

  _gizmoMesh->lookAt(_rootMesh->position().add(planeNormal));
  _rootMesh->addChild(*_gizmoMesh);
  _gizmoMesh->scaling().scaleInPlace(1.f / 3.f);

  // Add drag behavior to handle events when the gizmo is dragged
  PointerDragBehaviorOptions options;
  options.dragPlaneNormal    = planeNormal;
  dragBehavior               = std::make_unique<PointerDragBehavior>(options);
  dragBehavior->moveAttached = false;
  dragBehavior->maxDragAngle = PlaneRotationGizmo::MaxDragAngle;
  dragBehavior->_useAlternatePickedPointAboveMaxDragAngle = true;
  // _rootMesh->addBehavior(dragBehavior.get());

  dragBehavior->onDragStartObservable.add([&](DragStartOrEndEvent* e, EventState& /*es*/) {
    if (attachedNode()) {
      _lastDragPosition.copyFrom(e->dragPlanePoint);

      // This is for instantiation location of rotation circle
      const auto forward = Vector3(0.f, 0.f, 1.f);
      auto direction     = _rotationCircle->getDirection(forward);
      direction.normalize();

      // Remove Rotation Circle from parent mesh before drag interaction
      _gizmoMesh->removeChild(*_rotationCircle);

      _lastDragPosition.copyFrom(e->dragPlanePoint);
      _dragPlanePoint   = e->dragPlanePoint;
      const auto origin = _rotationCircle->getAbsolutePosition();
      const auto originalRotationPoint
        = _rotationCircle->getAbsolutePosition().clone()->addInPlace(direction);
      const auto dragStartPoint = e->dragPlanePoint;
      const auto angle
        = Vector3::GetAngleBetweenVectors(originalRotationPoint.subtract(origin),
                                          dragStartPoint.subtract(origin), _rotationCircle->up());

      _rotationCircle->addRotation(0.f, angle, 0.f);
      _dragging = true;
    }
  });

  dragBehavior->onDragEndObservable.add([&](DragStartOrEndEvent* /*event*/, EventState& /*es*/) {
    _dragDistance = 0.f;
    updateRotationCircle(_rotationCircle, rotationCirclePaths, _dragDistance, _dragPlanePoint);
    // Add rotation circle back to parent mesh after drag behavior
    _gizmoMesh->addChild(*_rotationCircle);
    _dragging = false;
  });

  _currentSnapDragDistance = 0.f;
  dragBehavior->onDragObservable.add([&](DragMoveEvent* event, EventState& /*es*/) {
    if (attachedNode()) {
      // Calc angle over full 360 degree
      // (https://stackoverflow.com/questions/43493711/the-angle-between-two-3d-vectors-with-a-result-range-0-360)
      std::optional<Vector3> nodeScale         = Vector3(1.f, 1.f, 1.f);
      std::optional<Quaternion> nodeQuaternion = Quaternion(0.f, 0.f, 0.f, 1.f);
      std::optional<Vector3> nodeTranslation   = Vector3(0.f, 0.f, 0.f);
      attachedNode()->getWorldMatrix().decompose(nodeScale, nodeQuaternion, nodeTranslation);

      auto newVector      = event->dragPlanePoint.subtract(*nodeTranslation).normalize();
      auto originalVector = _lastDragPosition.subtract(*nodeTranslation).normalize();
      auto cross          = Vector3::Cross(newVector, originalVector);
      auto dot            = Vector3::Dot(newVector, originalVector);
      auto angle          = std::atan2(cross.length(), dot);
      _planeNormalTowardsCamera.copyFrom(planeNormal);
      _localPlaneNormalTowardsCamera.copyFrom(planeNormal);
      if (updateGizmoRotationToMatchAttachedMesh) {
        nodeQuaternion->toRotationMatrix(_rotationMatrix);
        _localPlaneNormalTowardsCamera
          = Vector3::TransformCoordinates(_planeNormalTowardsCamera, _rotationMatrix);
      }
      // Flip up vector depending on which side the camera is on
      auto cameraFlipped = false;
      if (gizmoLayer->utilityLayerScene->activeCamera()) {
        auto camVec
          = gizmoLayer->utilityLayerScene->activeCamera()->position().subtract(*nodeTranslation);
        if (Vector3::Dot(camVec, _localPlaneNormalTowardsCamera) > 0.f) {
          _planeNormalTowardsCamera.scaleInPlace(-1.f);
          _localPlaneNormalTowardsCamera.scaleInPlace(-1.f);
          cameraFlipped = true;
        }
      }
      auto halfCircleSide = Vector3::Dot(_localPlaneNormalTowardsCamera, cross) > 0.f;
      if (halfCircleSide) {
        angle = -angle;
      }

      // Snapping logic
      auto snapped = false;
      if (snapDistance != 0.f) {
        _currentSnapDragDistance += angle;
        if (std::abs(_currentSnapDragDistance) > snapDistance) {
          auto dragSteps = std::floor(std::abs(_currentSnapDragDistance) / snapDistance);
          if (_currentSnapDragDistance < 0.f) {
            dragSteps *= -1.f;
          }
          _currentSnapDragDistance = std::fmod(_currentSnapDragDistance, snapDistance);
          angle                    = snapDistance * dragSteps;
          snapped                  = true;
        }
        else {
          angle = 0.f;
        }
      }

      _dragDistance += cameraFlipped ? -angle : angle;
      updateRotationCircle(_rotationCircle, rotationCirclePaths, _dragDistance, _dragPlanePoint);

      // Convert angle and axis to quaternion
      // (http://www.euclideanspace.com/maths/geometry/rotations/conversions/angleToQuaternion/index.htm)
      auto quaternionCoefficient = std::sin(angle / 2.f);
      _amountToRotate.set(_planeNormalTowardsCamera.x * quaternionCoefficient,
                          _planeNormalTowardsCamera.y * quaternionCoefficient,
                          _planeNormalTowardsCamera.z * quaternionCoefficient,
                          std::cos(angle / 2.f));

      // If the meshes local scale is inverted (eg. loaded gltf file parent with z scale of -1) the
      // rotation needs to be inverted on the y axis
      if (_tmpMatrix.determinant() > 0.f) {
        auto iTmpVector = Vector3();
        _amountToRotate.toEulerAnglesToRef(iTmpVector);
        Quaternion::RotationYawPitchRollToRef(iTmpVector.y, -iTmpVector.x, -iTmpVector.z,
                                              _amountToRotate);
      }

      if (updateGizmoRotationToMatchAttachedMesh) {
        // Rotate selected mesh quaternion over fixed axis
        nodeQuaternion->multiplyToRef(_amountToRotate, *nodeQuaternion);
      }
      else {
        // Rotate selected mesh quaternion over rotated axis
        _amountToRotate.multiplyToRef(*nodeQuaternion, *nodeQuaternion);
      }

      // recompose matrix
      attachedNode()->getWorldMatrix().copyFrom(
        Matrix::Compose(*nodeScale, *nodeQuaternion, *nodeTranslation));

      _lastDragPosition.copyFrom(event->dragPlanePoint);
      if (snapped) {
        _tmpSnapEvent.snapDistance = angle;
        onSnapObservable.notifyObservers(&_tmpSnapEvent);
      }
      _matrixChanged();
    }
  });

  auto& light = gizmoLayer->_getSharedGizmoLight();
  light->includedOnlyMeshes
    = stl_util::concat(light->includedOnlyMeshes(), _rootMesh->getChildMeshes(false));

  _cache.colliderMeshes  = {_collider};
  _cache.gizmoMeshes     = {_rotationMesh};
  _cache.material        = _coloredMaterial;
  _cache.hoverMaterial   = _hoverMaterial;
  _cache.disableMaterial = _disableMaterial;
  _cache.active          = false;
  if (_parent) {
    _parent->addToAxisCache(static_cast<Mesh*>(_gizmoMesh.get()), _cache);
  }

  _pointerObserver = gizmoLayer->utilityLayerScene->onPointerObservable.add(
    [&](PointerInfo* pointerInfo, EventState& /*es*/) {
      if (_customMeshSet || !pointerInfo) {
        return;
      }
      auto pickedMesh = std::static_pointer_cast<Mesh>(pointerInfo->pickInfo.pickedMesh);
      _isHovered      = stl_util::contains(_cache.colliderMeshes, pickedMesh);
      if (!_parent) {
        auto material = _isHovered || _dragging ? _hoverMaterial : _coloredMaterial;
        for (auto& m : _cache.gizmoMeshes) {
          m->material         = material;
          const auto lineMesh = std::static_pointer_cast<LinesMesh>(m);
          if (lineMesh) {
            lineMesh->color = material->diffuseColor;
          }
        }
      }
    });
}

PlaneRotationGizmo::~PlaneRotationGizmo() = default;

std::tuple<MeshPtr, MeshPtr> PlaneRotationGizmo::_createGizmoMesh(const AbstractMeshPtr& parentMesh,
                                                                  float thickness,
                                                                  unsigned int tessellation)
{
  auto collider          = Mesh::CreateTorus("ignore", 0.6f, 0.03f * thickness, tessellation,
                                    gizmoLayer->utilityLayerScene.get());
  collider->visibility   = 0.f;
  auto rotationMesh      = Mesh::CreateTorus("", 0.6f, 0.005f * thickness, tessellation,
                                        gizmoLayer->utilityLayerScene.get());
  rotationMesh->material = _coloredMaterial;

  // Position arrow pointing in its drag axis
  rotationMesh->rotation().x = Math::PI / 2.f;
  collider->rotation().x     = Math::PI / 2.f;

  parentMesh->addChild(*rotationMesh);
  parentMesh->addChild(*collider);
  return {rotationMesh, collider};
}

void PlaneRotationGizmo::_attachedNodeChanged(const NodePtr& value)
{
  if (dragBehavior) {
    dragBehavior->enabled = value ? true : false;
  }
}

MeshPtr PlaneRotationGizmo::setupRotationCircle(std::vector<std::vector<Vector3>>& paths,
                                                const AbstractMeshPtr& parentMesh)
{
  const auto fillRadians = 0.f;
  const auto step
    = PlaneRotationGizmo::_CircleConstants.pi2 / PlaneRotationGizmo::_CircleConstants.tessellation;
  for (auto p = -Math::PI / 2.f; p < Math::PI / 2.f - 1.5f; p += step / 2.f) {
    std::vector<Vector3> path;
    for (auto i = 0.f; i < PlaneRotationGizmo::_CircleConstants.pi2
                               * PlaneRotationGizmo::_CircleConstants.rotationCircleRange
                             + 0.01f;
         i += step) {
      if (i < fillRadians) {
        const auto x = PlaneRotationGizmo::_CircleConstants.radius * std::sin(i) * std::cos(p);
        const auto z = PlaneRotationGizmo::_CircleConstants.radius * std::cos(i) * std::cos(p);
        const auto y = 0;
        path.emplace_back(Vector3(x, y, z));
      }
      else {
        path.emplace_back(Vector3(0.f, 0.f, 0.f));
      }
    }

    paths.emplace_back(path);
  }

  const auto mat       = StandardMaterial::New("", gizmoLayer->utilityLayerScene.get());
  mat->diffuseColor    = Color3::Yellow();
  mat->backFaceCulling = false;
  const auto mesh      = Mesh::CreateRibbon("rotationCircle", paths, false, false, 0,
                                       gizmoLayer->utilityLayerScene.get(), true);
  mesh->material       = mat;
  mat->alpha           = 0.25f;
  mesh->rotation().x   = Math::PI / 2.f;
  parentMesh->addChild(*mesh);
  return mesh;
}

void PlaneRotationGizmo::updateRotationPath(std::vector<std::vector<Vector3>>& pathArr,
                                            float newFill)
{
  // To update the Ribbon, you have to mutate the pathArray in-place
  const auto step
    = PlaneRotationGizmo::_CircleConstants.pi2 / PlaneRotationGizmo::_CircleConstants.tessellation;
  auto tessellationCounter = 0u;
  for (auto p = -Math::PI / 2.f; p < Math::PI / 2.f - 1.5f; p += step / 2.f) {
    if (tessellationCounter < pathArr.size()) {
      auto& path         = pathArr[tessellationCounter];
      auto radianCounter = 0u;
      for (auto i = 0.f; i < PlaneRotationGizmo::_CircleConstants.pi2
                                 * PlaneRotationGizmo::_CircleConstants.rotationCircleRange
                               + 0.01f;
           i += step) {
        if (radianCounter < path.size()) {
          if (i < std::abs(newFill)) {
            const auto absI = (newFill > 0.f) ? i : i * -1.f;
            const auto absP = (newFill > 0.f) ? p : p * -1.f;
            path[radianCounter].set(
              PlaneRotationGizmo::_CircleConstants.radius * std::sin(absI) * std::cos(absP), 0.f,
              PlaneRotationGizmo::_CircleConstants.radius * std::cos(absI) * std::cos(absP));
          }
          else {
            path[radianCounter].set(0, 0, 0);
          }
        }

        ++radianCounter;
      }
    }

    ++tessellationCounter;
  }
}

void PlaneRotationGizmo::updateRotationCircle(const MeshPtr& mesh,
                                              std::vector<std::vector<Vector3>>& paths,
                                              float newFill, const Vector3& /*dragPlanePoint*/)
{
  updateRotationPath(paths, newFill);
  Mesh::CreateRibbon("rotationCircle", paths, false, false, 0, gizmoLayer->utilityLayerScene.get(),
                     std::nullopt, std::nullopt, mesh->geometry() ? mesh : nullptr);
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

void PlaneRotationGizmo::dispose(bool doNotRecurse, bool disposeMaterialAndTextures)
{
  onSnapObservable.clear();
  gizmoLayer->utilityLayerScene->onPointerObservable.remove(_pointerObserver);
  dragBehavior->detach();
  if (_gizmoMesh) {
    _gizmoMesh->dispose();
  }
  if (_rotationCircle) {
    _rotationCircle->dispose();
  }
  for (const auto& matl : {_coloredMaterial, _hoverMaterial, _disableMaterial}) {
    if (matl) {
      matl->dispose();
    }
  }
  Gizmo::dispose(doNotRecurse, disposeMaterialAndTextures);
}

} // end of namespace BABYLON
