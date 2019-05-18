#include <babylon/behaviors/meshes/multi_pointer_scale_behavior.h>

#include <babylon/behaviors/meshes/pointer_drag_behavior.h>
#include <babylon/engines/scene.h>
#include <babylon/meshes/mesh.h>

namespace BABYLON {

MultiPointerScaleBehavior::MultiPointerScaleBehavior()
    : _dragBehaviorA{std::make_unique<PointerDragBehavior>()}
    , _dragBehaviorB{std::make_unique<PointerDragBehavior>()}
    , _startDistance{0.f}
    , _initialScale{Vector3(0.f, 0.f, 0.f)}
    , _targetScale{Vector3(0.f, 0.f, 0.f)}
    , _ownerNode{nullptr}
    , _sceneRenderObserver{nullptr}
{
  _dragBehaviorA->moveAttached = false;
  _dragBehaviorB->moveAttached = false;
}

MultiPointerScaleBehavior::~MultiPointerScaleBehavior()
{
}

const std::string MultiPointerScaleBehavior::name() const
{
  return "MultiPointerScale";
}

void MultiPointerScaleBehavior::init()
{
}

float MultiPointerScaleBehavior::_getCurrentDistance()
{
  return _dragBehaviorA->lastDragPosition
    .subtract(_dragBehaviorB->lastDragPosition)
    .length();
}

void MultiPointerScaleBehavior::attach(const MeshPtr& ownerNode)
{
  _ownerNode = ownerNode;

  // Create 2 drag behaviors such that each will only be triggered by a separate
  // pointer
  _dragBehaviorA->onDragStartObservable.add(
    [this](DragStartOrEndEvent* /*event*/, EventState& /*es*/) {
      if (_dragBehaviorA->dragging && _dragBehaviorB->dragging) {
        if (_dragBehaviorA->currentDraggingPointerID
            == _dragBehaviorB->currentDraggingPointerID) {
          _dragBehaviorA->releaseDrag();
        }
        else {
          _initialScale.copyFrom(_ownerNode->scaling());
          _startDistance = _getCurrentDistance();
        }
      }
    });
  _dragBehaviorB->onDragStartObservable.add(
    [this](DragStartOrEndEvent* /*event*/, EventState& /*es*/) {
      if (_dragBehaviorA->dragging && _dragBehaviorB->dragging) {
        if (_dragBehaviorA->currentDraggingPointerID
            == _dragBehaviorB->currentDraggingPointerID) {
          _dragBehaviorB->releaseDrag();
        }
        else {
          _initialScale.copyFrom(_ownerNode->scaling());
          _startDistance = _getCurrentDistance();
        }
      }
    });

  // Once both drag behaviors are active scale based on the distance between the
  // two pointers
  for (auto& behavior : {_dragBehaviorA.get(), _dragBehaviorB.get()}) {
    behavior->onDragObservable.add(
      [this](DragMoveEvent* /*event*/, EventState& /*es*/) {
        if (_dragBehaviorA->dragging && _dragBehaviorB->dragging) {
          auto ratio = _getCurrentDistance() / _startDistance;
          _initialScale.scaleToRef(ratio, _targetScale);
        }
      });
  }

  // _ownerNode->addBehavior(_dragBehaviorA.get());
  // _ownerNode->addBehavior(_dragBehaviorB.get());

  // On every frame move towards target scaling to avoid jitter caused by vr
  // controllers
  _sceneRenderObserver = _ownerNode->getScene()->onBeforeRenderObservable.add(
    [this](Scene* /*scene*/, EventState& /*es*/) {
      if (_dragBehaviorA->dragging && _dragBehaviorB->dragging) {
        auto change
          = _targetScale.subtract(_ownerNode->scaling()).scaleInPlace(0.1f);
        if (change.length() > 0.01f) {
          _ownerNode->scaling().addInPlace(change);
        }
      }
    });
}

void MultiPointerScaleBehavior::detach()
{
  _ownerNode->getScene()->onBeforeRenderObservable.remove(_sceneRenderObserver);
  for (auto& behavior : {_dragBehaviorA.get(), _dragBehaviorB.get()}) {
    behavior->onDragStartObservable.clear();
    behavior->onDragObservable.clear();
    // _ownerNode->removeBehavior(behavior);
  }
}

} // end of namespace BABYLON
