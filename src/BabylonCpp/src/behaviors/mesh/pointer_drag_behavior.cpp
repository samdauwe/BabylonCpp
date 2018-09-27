#include <babylon/behaviors/mesh/pointer_drag_behavior.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

std::unique_ptr<Scene> PointerDragBehavior::_planeScene = nullptr;

PointerDragBehavior::PointerDragBehavior(
  const PointerDragBehaviorOptions& iOptions)
    : maxDragAngle{0.f}
    , _useAlternatePickedPointAboveMaxDragAngle{false}
    , dragging{false}
    , dragDeltaRatio{0.2f}
    , updateDragPlane{true}
    , moveAttached{true}
    , enabled{true}
    , detachCameraControls{true}
    , useObjectOrienationForDragging{true}
    , _attachedNode{nullptr}
    , _dragPlane{nullptr}
    , _scene{nullptr}
    , _pointerObserver{nullptr}
    , _beforeRenderObserver{nullptr}
    , _draggingID{-1}
    , _debugMode{false}
    , _moving{false}
    , _options{iOptions}
    , _tmpVector{Vector3{0.f, 0.f, 0.f}}
    , _alternatePickedPoint{Vector3{0.f, 0.f, 0.f}}
    , _worldDragAxis{Vector3{0.f, 0.f, 0.f}}
    , _targetPosition{Vector3{0.f, 0.f, 0.f}}
    , _attachedElement{nullptr}
    , _startDragRay{Ray(Vector3(), Vector3())}
    , _pointA{Vector3{0.f, 0.f, 0.f}}
    , _pointB{Vector3{0.f, 0.f, 0.f}}
    , _pointC{Vector3{0.f, 0.f, 0.f}}
    , _lineA{Vector3{0.f, 0.f, 0.f}}
    , _lineB{Vector3{0.f, 0.f, 0.f}}
    , _localAxis{Vector3{0.f, 0.f, 0.f}}
    , _lookAt{Vector3{0.f, 0.f, 0.f}}
{
  unsigned int optionCount = 0;
  if (_options.dragAxis) {
    ++optionCount;
  }
  if (_options.dragPlaneNormal) {
    ++optionCount;
  }
  if (optionCount > 1) {
    throw std::runtime_error(
      "Multiple drag modes specified in dragBehavior options. Only one "
      "expected");
  }
}

PointerDragBehavior::~PointerDragBehavior()
{
}

const std::string PointerDragBehavior::name() const
{
  return "PointerDrag";
}

void PointerDragBehavior::init()
{
}

void PointerDragBehavior::attach(const NodePtr& ownerNode)
{
  _scene        = ownerNode->getScene();
  _attachedNode = ownerNode;

  // Initialize drag plane to not interfere with existing scene
  if (!PointerDragBehavior::_planeScene) {
    if (_debugMode) {
      // PointerDragBehavior::_planeScene = _scene;
    }
    else {
      PointerDragBehavior::_planeScene = Scene::New(_scene->getEngine());
      PointerDragBehavior::_planeScene->detachControl();
      _scene->getEngine()->scenes.pop_back();
      _scene->onDisposeObservable.addOnce(
        [](Scene* /*scene*/, EventState& /*es*/) {
          PointerDragBehavior::_planeScene->dispose();
          PointerDragBehavior::_planeScene = nullptr;
        });
    }
  }
  _dragPlane = Mesh::CreatePlane("pointerDragPlane", _debugMode ? 1 : 10000,
                                 PointerDragBehavior::_planeScene.get(), false,
                                 Mesh::DOUBLESIDE());

  // State of the drag
  Vector3 lastPosition{0.f, 0.f, 0.f};

  const auto& pickPredicate = [this](const AbstractMeshPtr& m) -> bool {
    return _attachedNode == m || m->isDescendantOf(_attachedNode.get());
  };

  _pointerObserver = _scene->onPointerObservable.add(
    [&](PointerInfo* pointerInfo, EventState& /*es*/) {
      if (!enabled) {
        return;
      }

      const auto& _pickInfo = pointerInfo->pickInfo;
      if (pointerInfo->type == PointerEventTypes::POINTERDOWN) {
        if (!dragging && _pickInfo.hit && _pickInfo.pickedMesh
            && _pickInfo.pickedPoint && _pickInfo.ray
            && pickPredicate(_pickInfo.pickedMesh)) {
          _startDrag(pointerInfo->pointerEvent.pointerId,
                     pointerInfo->pickInfo.ray,
                     pointerInfo->pickInfo.pickedPoint);
        }
      }
      else if (pointerInfo->type == PointerEventTypes::POINTERUP) {
        if (currentDraggingPointerID == pointerInfo->pointerEvent.pointerId) {
          releaseDrag();
        }
      }
      else if (pointerInfo->type == PointerEventTypes::POINTERMOVE) {
        auto pointerId = pointerInfo->pointerEvent.pointerId;
        // Keep track of last pointer ray, this is used simulating the start of
        // a drag in startDrag()
        if (!stl_util::contains(_lastPointerRay, pointerId)) {
          _lastPointerRay[pointerId] = Ray(Vector3(), Vector3());
        }
        if (pointerInfo->pickInfo.ray) {
          _lastPointerRay[pointerId].origin.copyFrom(
            pointerInfo->pickInfo.ray->origin);
          _lastPointerRay[pointerId].direction.copyFrom(
            pointerInfo->pickInfo.ray->direction);
          if (currentDraggingPointerID == pointerId && dragging) {
            _moveDrag(*pointerInfo->pickInfo.ray);
          }
        }
      }
    });

  _beforeRenderObserver = _scene->onBeforeRenderObservable.add(
    [&](Scene* /*scene*/, EventState& /*es*/) {
      if (_moving && moveAttached) {
        // Slowly move mesh to avoid jitter
        _targetPosition.subtractToRef(
          std::static_pointer_cast<Mesh>(_attachedNode)->absolutePosition(),
          _tmpVector);
        _tmpVector.scaleInPlace(dragDeltaRatio);
        std::static_pointer_cast<Mesh>(_attachedNode)
          ->getAbsolutePosition()
          .addToRef(_tmpVector, _tmpVector);
        std::static_pointer_cast<Mesh>(_attachedNode)
          ->setAbsolutePosition(_tmpVector);
      }
    });
}

void PointerDragBehavior::releaseDrag()
{
  dragging = false;
  DragStartOrEndEvent event;
  event.dragPlanePoint = lastDragPosition;
  event.pointerId      = currentDraggingPointerID;
  onDragEndObservable.notifyObservers(&event);
  currentDraggingPointerID = -1;
  _moving                  = false;

  // Reattach camera controls
  if (detachCameraControls && _attachedElement && _scene->activeCamera
      && !_scene->activeCamera->leftCamera()) {
    _scene->activeCamera->attachControl(_attachedElement, true);
  }
}

void PointerDragBehavior::startDrag(
  int pointerId, const std::optional<Ray>& fromRay,
  const std::optional<Vector3>& startPickedPoint)
{
  _startDrag(pointerId, fromRay, startPickedPoint);
  if (stl_util::contains(_lastPointerRay, pointerId)) {
    // if there was a last pointer ray drag the object there
    _moveDrag(_lastPointerRay[pointerId]);
  }
}

void PointerDragBehavior::_startDrag(
  int /*pointerId*/, const std::optional<Ray>& fromRay,
  const std::optional<Vector3>& startPickedPoint)
{
  if (!_scene->activeCamera || dragging || !_attachedNode) {
    return;
  }

  // Create start ray from the camera to the object
  if (fromRay) {
    _startDragRay.direction.copyFrom(fromRay->direction);
    _startDragRay.origin.copyFrom(fromRay->origin);
  }
  else {
    _startDragRay.origin.copyFrom(_scene->activeCamera->position);
    _attachedNode->getWorldMatrix()->getTranslationToRef(_tmpVector);
    _tmpVector.subtractToRef(_scene->activeCamera->position,
                             _startDragRay.direction);
  }

  _updateDragPlanePosition(_startDragRay,
                           startPickedPoint ? *startPickedPoint : _tmpVector);

  auto pickedPoint = _pickWithRayOnDragPlane(_startDragRay);
  if (pickedPoint) {
    dragging                 = true;
    currentDraggingPointerID = 1;
    lastDragPosition.copyFrom(*pickedPoint);

    DragStartOrEndEvent dragStartOrEndEvent;
    dragStartOrEndEvent.dragPlanePoint = *pickedPoint;
    dragStartOrEndEvent.pointerId      = currentDraggingPointerID;

    onDragStartObservable.notifyObservers(&dragStartOrEndEvent);
    _targetPosition.copyFrom(
      std::dynamic_pointer_cast<Mesh>(_attachedNode)->absolutePosition);

    // Detatch camera controls
    if (detachCameraControls && _scene->activeCamera
        && !_scene->activeCamera->leftCamera()) {
      if (_scene->activeCamera->inputs.attachedElement) {
        _attachedElement = _scene->activeCamera->inputs.attachedElement;
        _scene->activeCamera->detachControl(
          _scene->activeCamera->inputs.attachedElement);
      }
      else {
        _attachedElement = nullptr;
      }
    }
  }
}

void PointerDragBehavior::_moveDrag(const Ray& ray)
{
  _moving          = true;
  auto pickedPoint = _pickWithRayOnDragPlane(ray);

  if (pickedPoint) {
    if (updateDragPlane) {
      _updateDragPlanePosition(ray, *pickedPoint);
    }

    auto dragLength = 0.f;
    // depending on the drag mode option drag accordingly
    if (_options.dragAxis) {
      // Convert local drag axis to world
      Vector3::TransformCoordinatesToRef(
        *_options.dragAxis,
        _attachedNode->getWorldMatrix()->getRotationMatrix(), _worldDragAxis);

      // Project delta drag from the drag plane onto the drag axis
      pickedPoint->subtractToRef(lastDragPosition, _tmpVector);
      dragLength = Vector3::Dot(_tmpVector, _worldDragAxis);
      _worldDragAxis.scaleToRef(dragLength, _dragDelta);
    }
    else {
      dragLength = _dragDelta.length();
      pickedPoint->subtractToRef(lastDragPosition, _dragDelta);
    }
    _targetPosition.addInPlace(_dragDelta);

    DragMoveEvent dragMoveEvent;
    dragMoveEvent.dragDistance    = dragLength;
    dragMoveEvent.delta           = _dragDelta;
    dragMoveEvent.dragPlanePoint  = *pickedPoint;
    dragMoveEvent.dragPlaneNormal = _dragPlane->forward;
    dragMoveEvent.pointerId       = currentDraggingPointerID;

    onDragObservable.notifyObservers(&dragMoveEvent);
    lastDragPosition.copyFrom(*pickedPoint);
  }
}

std::optional<Vector3>
PointerDragBehavior::_pickWithRayOnDragPlane(const std::optional<Ray>& ray)
{
  if (!ray) {
    return std::nullopt;
  }

  // Calculate angle between plane normal and ray
  auto angle
    = std::acos(Vector3::Dot(_dragPlane->forward(), (*ray).direction));
  // Correct if ray is casted from oposite side
  if (angle > Math::PI_2) {
    angle = Math::PI - angle;
  }

  // If the angle is too perpendicular to the plane pick another point on the
  // plane where it is looking
  if (maxDragAngle > 0.f && angle > maxDragAngle) {
    if (_useAlternatePickedPointAboveMaxDragAngle) {
      // Invert ray direction along the towards object axis
      _tmpVector.copyFrom((*ray).direction);
      std::static_pointer_cast<Mesh>(_attachedNode)
        ->absolutePosition()
        .subtractToRef((*ray).origin, _alternatePickedPoint);
      _alternatePickedPoint.normalize();
      _alternatePickedPoint.scaleInPlace(
        -2.f * Vector3::Dot(_alternatePickedPoint, _tmpVector));
      _tmpVector.addInPlace(_alternatePickedPoint);

      // Project resulting vector onto the drag plane and add it to the attached
      // nodes absolute position to get a picked point
      auto dot = Vector3::Dot(_dragPlane->forward(), _tmpVector);
      _dragPlane->forward().scaleToRef(-dot, _alternatePickedPoint);
      _alternatePickedPoint.addInPlace(_tmpVector);
      _alternatePickedPoint.addInPlace(
        std::static_pointer_cast<Mesh>(_attachedNode)->absolutePosition());
      return _alternatePickedPoint;
    }
    else {
      return std::nullopt;
    }
  }

  auto pickResult = PointerDragBehavior::_planeScene->pickWithRay(
    *ray, [this](const AbstractMeshPtr& m) -> bool { return m == _dragPlane; });
  if (pickResult && (*pickResult).hit && (*pickResult).pickedMesh
      && (*pickResult).pickedPoint) {
    return (*pickResult).pickedPoint;
  }
  else {
    return std::nullopt;
  }
}

void PointerDragBehavior::_updateDragPlanePosition(
  const Ray& ray, const Vector3& dragPlanePosition)
{
  _pointA.copyFrom(dragPlanePosition);
  if (_options.dragAxis) {
    /*useObjectOrienationForDragging ?
      Vector3::TransformCoordinatesToRef(
        *_options.dragAxis,
      _attachedNode->getWorldMatrix()->getRotationMatrix(), _localAxis) :
      _localAxis.copyFrom(*_options.dragAxis);*/

    // Calculate plane normal in direction of camera but perpendicular to drag
    // axis
    _pointA.addToRef(_localAxis, _pointB); // towards drag axis
    ray.origin.subtractToRef(_pointA, _pointC);
    _pointA.addToRef(_pointC.normalize(), _pointC); // towards camera
    // Get perpendicular line from direction to camera and drag axis
    _pointB.subtractToRef(_pointA, _lineA);
    _pointC.subtractToRef(_pointA, _lineB);
    Vector3::CrossToRef(_lineA, _lineB, _lookAt);
    // Get perpendicular line from previous result and drag axis to adjust lineB
    // to be perpendiculat to camera
    Vector3::CrossToRef(_lineA, _lookAt, _lookAt);
    _lookAt.normalize();

    _dragPlane->position().copyFrom(_pointA);
    _pointA.subtractToRef(_lookAt, _lookAt);
    _dragPlane->lookAt(_lookAt);
  }
  else if (_options.dragPlaneNormal) {
    /*useObjectOrienationForDragging ?
      Vector3::TransformCoordinatesToRef(
        *_options.dragPlaneNormal,
        _attachedNode.getWorldMatrix()->getRotationMatrix(), _localAxis) :
      _localAxis.copyFrom(*_options.dragPlaneNormal);*/
    _dragPlane->position().copyFrom(_pointA);
    _pointA.subtractToRef(_localAxis, _lookAt);
    _dragPlane->lookAt(_lookAt);
  }
  else {
    _dragPlane->position().copyFrom(_pointA);
    _dragPlane->lookAt(ray.origin);
  }
  _dragPlane->computeWorldMatrix(true);
}

void PointerDragBehavior::detach()
{
  if (_pointerObserver) {
    // _scene->onPrePointerObservable.remove(_pointerObserver);
  }
  if (_beforeRenderObserver) {
    _scene->onBeforeRenderObservable.remove(_beforeRenderObserver);
  }
}

} // end of namespace BABYLON
