#include <babylon/behaviors/mesh/pointer_drag_behavior.h>

#include <babylon/culling/ray.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

unique_ptr_t<Scene> PointerDragBehavior::_planeScene = nullptr;

PointerDragBehavior::PointerDragBehavior(
  const PointerDragBehaviorOptions& iOptions)
    : currentDraggingPointerID{-1}
    , dragging{false}
    , dragDeltaRatio{0.2f}
    , updateDragPlane{true}
    , moveAttached{true}
    , enabled{true}
    , useObjectOrienationForDragging{true}
    , _attachedNode{nullptr}
    , _dragPlane{nullptr}
    , _scene{nullptr}
    , _pointerObserver{nullptr}
    , _draggingID{-1}
    , _debugMode{false}
    , _moving{false}
    , options{iOptions}
    , _tmpVector{Vector3{0.f, 0.f, 0.f}}
    , _worldDragAxis{Vector3{0.f, 0.f, 0.f}}
    , _pointA{Vector3{0.f, 0.f, 0.f}}
    , _pointB{Vector3{0.f, 0.f, 0.f}}
    , _pointC{Vector3{0.f, 0.f, 0.f}}
    , _lineA{Vector3{0.f, 0.f, 0.f}}
    , _lineB{Vector3{0.f, 0.f, 0.f}}
    , _localAxis{Vector3{0.f, 0.f, 0.f}}
    , _lookAt{Vector3{0.f, 0.f, 0.f}}
{
  unsigned int optionCount = 0;
  if (options.dragAxis) {
    ++optionCount;
  }
  if (options.dragPlaneNormal) {
    ++optionCount;
  }
  if (optionCount > 1) {
    throw ::std::runtime_error(
      "Multiple drag modes specified in dragBehavior options. Only one "
      "expected");
  }
}

PointerDragBehavior::~PointerDragBehavior()
{
}

const string_t PointerDragBehavior::name() const
{
  return "PointerDrag";
}

void PointerDragBehavior::init()
{
}

void PointerDragBehavior::attach(Node* ownerNode)
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
      _scene->getEngine()->scenes.pop_back();
    }
  }
  _dragPlane = Mesh::CreatePlane("pointerDragPlane", _debugMode ? 1 : 10000,
                                 PointerDragBehavior::_planeScene.get(), false,
                                 Mesh::DOUBLESIDE());

  // State of the drag
  Vector3 lastPosition{0.f, 0.f, 0.f};
  Vector3 delta{0.f, 0.f, 0.f};
  float dragLength = 0.f;
  Vector3 targetPosition{0.f, 0.f, 0.f};

  const auto& pickPredicate = [this](AbstractMesh* m) -> bool {
    return _attachedNode == m || m->isDescendantOf(_attachedNode);
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
          _updateDragPlanePosition(*_pickInfo.ray, *_pickInfo.pickedPoint);
          auto pickedPoint = _pickWithRayOnDragPlane(_pickInfo.ray);
          if (pickedPoint) {
            dragging                 = true;
            currentDraggingPointerID = pointerInfo->pointerEvent.pointerId;
            lastPosition.copyFrom(*pickedPoint);
            DragStartOrEndEvent event;
            event.dragPlanePoint = *pickedPoint;
            event.pointerId      = currentDraggingPointerID;
            onDragStartObservable.notifyObservers(&event);
          }
        }
      }
      else if (pointerInfo->type == PointerEventTypes::POINTERUP) {
        if (currentDraggingPointerID == pointerInfo->pointerEvent.pointerId) {
          releaseDrag();
        }
      }
      else if (pointerInfo->type == PointerEventTypes::POINTERMOVE) {
        if (currentDraggingPointerID == pointerInfo->pointerEvent.pointerId
            && dragging && _pickInfo.ray) {
          _moving          = true;
          auto pickedPoint = _pickWithRayOnDragPlane(_pickInfo.ray);

          if (pickedPoint) {
            if (updateDragPlane) {
              _updateDragPlanePosition(*_pickInfo.ray, *pickedPoint);
            }

            // depending on the drag mode option drag accordingly
            if (options.dragAxis) {
              // Convert local drag axis to world
              Vector3::TransformCoordinatesToRef(
                *options.dragAxis,
                _attachedNode->getWorldMatrix()->getRotationMatrix(),
                _worldDragAxis);

              // Project delta drag from the drag plane onto the drag axis
              (*pickedPoint).subtractToRef(lastDragPosition, _tmpVector);
              dragLength = Vector3::Dot(_tmpVector, _worldDragAxis);
              _worldDragAxis.scaleToRef(dragLength, delta);
            }
            else {
              dragLength = delta.length();
              (*pickedPoint).subtractToRef(lastDragPosition, delta);
            }
            targetPosition.addInPlace(delta);
            DragMoveEvent event;
            event.dragDistance    = dragLength;
            event.delta           = delta;
            event.dragPlanePoint  = *pickedPoint;
            event.dragPlaneNormal = _dragPlane->forward();
            event.pointerId       = currentDraggingPointerID;
            onDragObservable.notifyObservers(&event);
            lastPosition.copyFrom(*pickedPoint);
          }
        }
      }
    });

  _scene->onBeforeRenderObservable.add(
    [&](Scene* /*scene*/, EventState& /*es*/) {
      if (_moving && moveAttached) {
        // Slowly move mesh to avoid jitter
        targetPosition.subtractToRef(
          static_cast<Mesh*>(_attachedNode)->absolutePosition(), _tmpVector);
        _tmpVector.scaleInPlace(0.2f);
        static_cast<Mesh*>(_attachedNode)
          ->getAbsolutePosition()
          .addToRef(_tmpVector, _tmpVector);
        static_cast<Mesh*>(_attachedNode)->setAbsolutePosition(_tmpVector);
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
}

Nullable<Vector3>
PointerDragBehavior::_pickWithRayOnDragPlane(const Nullable<Ray>& ray)
{
  if (!ray) {
    return nullptr;
  }
  auto pickResult = PointerDragBehavior::_planeScene->pickWithRay(
    *ray, [this](AbstractMesh* m) -> bool { return m == _dragPlane; });
  if (pickResult && (*pickResult).hit && (*pickResult).pickedMesh
      && (*pickResult).pickedPoint) {
    return (*pickResult).pickedPoint;
  }
  else {
    return nullptr;
  }
}

void PointerDragBehavior::_updateDragPlanePosition(
  const Ray& ray, const Vector3& dragPlanePosition)
{
  _pointA.copyFrom(dragPlanePosition);
  if (options.dragAxis) {
    /*useObjectOrienationForDragging ?
      Vector3::TransformCoordinatesToRef(
        *options.dragAxis, _attachedNode->getWorldMatrix()->getRotationMatrix(),
        _localAxis) :
      _localAxis.copyFrom(*options.dragAxis);*/

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
  else if (options.dragPlaneNormal) {
    /*useObjectOrienationForDragging ?
      Vector3::TransformCoordinatesToRef(
        *options.dragPlaneNormal,
        _attachedNode.getWorldMatrix()->getRotationMatrix(), _localAxis) :
      _localAxis.copyFrom(*options.dragPlaneNormal);*/
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
}

} // end of namespace BABYLON
