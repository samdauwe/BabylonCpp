#include <babylon/behaviors/mesh/pointer_drag_behavior.h>

#include <babylon/culling/ray.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

unique_ptr_t<Scene> PointerDragBehavior::_planeScene = nullptr;

PointerDragBehavior::PointerDragBehavior(
  const PointerDragBehaviorOptions& iOptions)
    : moveAttached{true}
    , _dragPlaneParent{nullptr}
    , _attachedNode{nullptr}
    , _dragPlane{nullptr}
    , _scene{nullptr}
    , _pointerObserver{nullptr}
    , _draggingID{-1}
    , options{iOptions}
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
  if (optionCount < 1) {
    throw ::std::runtime_error(
      "At least one drag mode option must be specified");
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
  _scene = ownerNode->getScene();
  if (!options.pointerObservableScene) {
    options.pointerObservableScene = _scene;
  }
  _attachedNode = ownerNode;

  // Initialize drag plane to not interfere with existing scene
  if (!PointerDragBehavior::_planeScene) {
    PointerDragBehavior::_planeScene = Scene::New(_scene->getEngine());
    _scene->getEngine()->scenes.pop_back();
  }
  _dragPlane = Mesh::CreatePlane("pointerDragPlane", 1000,
                                 PointerDragBehavior::_planeScene.get(), false,
                                 Mesh::DOUBLESIDE());

  // State of the drag
  bool dragging = false;
  Vector3 lastPosition{0.f, 0.f, 0.f};
  Vector3 delta{0.f, 0.f, 0.f};

  const auto& pickPredicate = [this](AbstractMesh* m) -> bool {
    return _attachedNode == m || m->isDescendantOf(_attachedNode);
  };

  _pointerObserver = options.pointerObservableScene->onPrePointerObservable.add(
    [&](PointerInfoPre* pointerInfoPre, EventState& eventState) {
      // Check if attached mesh is picked
      auto pickInfo
        = pointerInfoPre->ray ?
            _scene->pickWithRay(*pointerInfoPre->ray, pickPredicate) :
            _scene->pick(_scene->pointerX(), _scene->pointerY(), pickPredicate);
      if (pickInfo) {
        auto _pickInfo = *pickInfo;
        _pickInfo.ray  = pointerInfoPre->ray;
        if (!_pickInfo.ray) {
          auto matIdentity = Matrix::Identity();
          _pickInfo.ray    = options.pointerObservableScene->createPickingRay(
            _scene->pointerX(), _scene->pointerY(), &matIdentity,
            _scene->activeCamera);
        }
        pickInfo = _pickInfo;
        if (_pickInfo.hit) {
          eventState.skipNextObservers = true;
        }
      }

      const auto& _pickInfo = *pickInfo;
      if (pointerInfoPre->type == PointerEventTypes::POINTERDOWN) {
        if (!dragging && pickInfo && _pickInfo.hit && _pickInfo.pickedMesh
            && _pickInfo.ray) {
          _updateDragPlanePosition(*_pickInfo.ray);
          auto pickedPoint = _pickWithRayOnDragPlane(_pickInfo.ray);
          if (pickedPoint) {
            dragging    = true;
            _draggingID = pointerInfoPre->pointerEvent.pointerId;
            lastPosition.copyFrom(*pickedPoint);
            DragStartOrEndEvent d;
            d.dragPlanePoint = *pickedPoint;
            onDragStartObservable.notifyObservers(&d);
          }
        }
      }
      else if (pointerInfoPre->type == PointerEventTypes::POINTERUP) {
        if (_draggingID == pointerInfoPre->pointerEvent.pointerId) {
          dragging    = false;
          _draggingID = -1;
          DragStartOrEndEvent d;
          d.dragPlanePoint = lastPosition;
          onDragEndObservable.notifyObservers(&d);
        }
      }
      else if (pointerInfoPre->type == PointerEventTypes::POINTERMOVE) {
        if (_draggingID == pointerInfoPre->pointerEvent.pointerId && dragging
            && pickInfo && _pickInfo.ray) {
          auto pickedPoint = _pickWithRayOnDragPlane(_pickInfo.ray);
          _updateDragPlanePosition(*_pickInfo.ray);
          if (pickedPoint) {
            // depending on the drag mode option drag accordingly
            if (options.dragAxis) {
              // get the closest point on the dragaxis from the selected mesh to
              // the picked point location
              // https://www.opengl.org/discussion_boards/showthread.php/159717-Closest-point-on-a-Vector-to-a-point
              (*options.dragAxis)
                .scaleToRef(Vector3::Dot((*pickedPoint).subtract(lastPosition),
                                         *options.dragAxis),
                            delta);
            }
            else {
              (*pickedPoint).subtractToRef(lastPosition, delta);
            }
            if (moveAttached) {
              static_cast<Mesh*>(_attachedNode)->position().addInPlace(delta);
            }
            DragMoveEvent d;
            d.delta          = delta;
            d.dragPlanePoint = lastPosition;
            onDragObservable.notifyObservers(&d);
            lastPosition.copyFrom(*pickedPoint);
          }
        }
      }
    });
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

void PointerDragBehavior::_updateDragPlanePosition(const Ray& ray)
{
  const auto& pointA
    = _dragPlaneParent ?
        _dragPlaneParent->position() :
        static_cast<Mesh*>(_attachedNode)->position(); // center
  if (options.dragAxis) {
    const auto& camPos = ray.origin;

    // Calculate plane normal in direction of camera but perpendicular to drag
    // axis
    auto pointB = pointA.add(*options.dragAxis); // towards drag axis
    auto pointC
      = pointA.add(camPos.subtract(pointA).normalize()); // towards camera
    // Get perpendicular line from direction to camera and drag axis
    auto lineA    = pointB.subtract(pointA);
    auto lineB    = pointC.subtract(pointA);
    auto perpLine = Vector3::Cross(lineA, lineB);
    // Get perpendicular line from previous result and drag axis to adjust lineB
    // to be perpendiculat to camera
    auto norm = Vector3::Cross(lineA, perpLine).normalize();

    _dragPlane->position().copyFrom(pointA);
    _dragPlane->lookAt(pointA.add(norm));
  }
  else if (options.dragPlaneNormal) {
    _dragPlane->position().copyFrom(pointA);
    _dragPlane->lookAt(pointA.add(*options.dragPlaneNormal));
  }
  _dragPlane->computeWorldMatrix(true);
}

void PointerDragBehavior::detach()
{
  if (_pointerObserver) {
    _scene->onPrePointerObservable.remove(_pointerObserver);
  }
}

} // end of namespace BABYLON
