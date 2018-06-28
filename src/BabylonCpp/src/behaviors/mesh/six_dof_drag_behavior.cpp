#include <babylon/behaviors/mesh/six_dof_drag_behavior.h>

#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

unique_ptr_t<Scene> SixDofDragBehavior::_virtualScene = nullptr;

SixDofDragBehavior::SixDofDragBehavior()
    : dragging{false}
    , dragDeltaRatio{0.2f}
    , currentDraggingPointerID{-1}
    , _ownerNode{nullptr}
    , _sceneRenderObserver{nullptr}
    , _scene{nullptr}
    , _targetPosition{Vector3(0.f, 0.f, 0.f)}
    , _virtualOriginMesh{nullptr}
    , _virtualDragMesh{nullptr}
    , _pointerObserver{nullptr}
    , _moving{false}
    , zDragFactor{5.f}
{
}

SixDofDragBehavior::~SixDofDragBehavior()
{
}

const string_t SixDofDragBehavior::name() const
{
  return "SixDofDrag";
}

void SixDofDragBehavior::init()
{
}

void SixDofDragBehavior::attach(Mesh* ownerNode)
{
  _ownerNode = ownerNode;
  _scene     = _ownerNode->getScene();
  if (!SixDofDragBehavior::_virtualScene) {
    SixDofDragBehavior::_virtualScene = Scene::New(_scene->getEngine());
    _scene->getEngine()->scenes.pop_back();
  }

  AbstractMesh* pickedMesh = nullptr;
  Vector3 lastSixDofOriginPosition(0, 0, 0);

  // Setup virtual meshes to be used for dragging without dirtying the existing
  // scene
  _virtualOriginMesh
    = AbstractMesh::New("", SixDofDragBehavior::_virtualScene.get());
  _virtualOriginMesh->setRotationQuaternion(Quaternion());
  _virtualDragMesh
    = AbstractMesh::New("", SixDofDragBehavior::_virtualScene.get());
  _virtualDragMesh->setRotationQuaternion(Quaternion());

  const auto pickPredicate = [this](AbstractMesh* m) {
    return _ownerNode == m || m->isDescendantOf(_ownerNode);
  };

  _pointerObserver = _scene->onPointerObservable.add([&](
                                                       PointerInfo* pointerInfo,
                                                       EventState& /*es*/) {
    if (pointerInfo->type == PointerEventTypes::POINTERDOWN) {
      if (!dragging && pointerInfo->pickInfo.hit
          && pointerInfo->pickInfo.pickedMesh && pointerInfo->pickInfo.ray
          && pickPredicate(pointerInfo->pickInfo.pickedMesh)) {
        pickedMesh = pointerInfo->pickInfo.pickedMesh;
        lastSixDofOriginPosition.copyFrom((*pointerInfo->pickInfo.ray).origin);

        // Set position and orientation of the controller
        _virtualOriginMesh->position().copyFrom(
          (*pointerInfo->pickInfo.ray).origin);
        _virtualOriginMesh->lookAt(
          (*pointerInfo->pickInfo.ray)
            .origin.subtract((*pointerInfo->pickInfo.ray).direction));

        // Attach the virtual drag mesh to the virtual origin mesh so it can be
        // dragged
        _virtualOriginMesh->removeChild(_virtualDragMesh);
        _virtualDragMesh->position().copyFrom(pickedMesh->absolutePosition());
        if (!pickedMesh->rotationQuaternion()) {
          pickedMesh->setRotationQuaternion(Quaternion::RotationYawPitchRoll(
            pickedMesh->rotation().y, pickedMesh->rotation().x,
            pickedMesh->rotation().z));
        }
        auto oldParent = pickedMesh->parent();
        pickedMesh->setParent(nullptr);
        _virtualDragMesh->rotationQuaternion()->copyFrom(
          *pickedMesh->rotationQuaternion());
        pickedMesh->setParent(oldParent);
        _virtualOriginMesh->addChild(_virtualDragMesh);

        // Update state
        _targetPosition.copyFrom(_virtualDragMesh->absolutePosition());
        dragging                 = true;
        currentDraggingPointerID = (pointerInfo->pointerEvent).pointerId;
      }
    }
    else if (pointerInfo->type == PointerEventTypes::POINTERUP) {
      if (currentDraggingPointerID == (pointerInfo->pointerEvent).pointerId) {
        dragging                 = false;
        _moving                  = false;
        currentDraggingPointerID = -1;
        pickedMesh               = nullptr;
        _virtualOriginMesh->removeChild(_virtualDragMesh);
      }
    }
    else if (pointerInfo->type == PointerEventTypes::POINTERMOVE) {
      if (currentDraggingPointerID == (pointerInfo->pointerEvent).pointerId
          && dragging && pointerInfo->pickInfo.ray && pickedMesh) {
        // Calculate controller drag distance in controller space
        auto originDragDifference
          = (*pointerInfo->pickInfo.ray)
              .origin.subtract(lastSixDofOriginPosition);
        lastSixDofOriginPosition.copyFrom((*pointerInfo->pickInfo.ray).origin);
        auto rotMat = _virtualOriginMesh->getWorldMatrix()->getRotationMatrix();
        auto localOriginDragDifference = Vector3::TransformCoordinates(
          originDragDifference, Matrix::Invert(rotMat));

        _virtualOriginMesh->addChild(_virtualDragMesh);
        // Determine how much the controller moved to/away towards the dragged
        // object and use this to move the object further when its further away
        auto zDragDistance
          = Vector3::Dot(localOriginDragDifference,
                         _virtualOriginMesh->position().normalizeToNew());
        _virtualDragMesh->position().z
          -= _virtualDragMesh->position().z < 1 ?
               zDragDistance * zDragFactor :
               zDragDistance * zDragFactor * _virtualDragMesh->position().z;
        if (_virtualDragMesh->position().z < 0) {
          _virtualDragMesh->position().z = 0;
        }

        // Update the controller position
        _virtualOriginMesh->position().copyFrom(
          (*pointerInfo->pickInfo.ray).origin);
        _virtualOriginMesh->lookAt(
          (*pointerInfo->pickInfo.ray)
            .origin.subtract((*pointerInfo->pickInfo.ray).direction));
        _virtualOriginMesh->removeChild(_virtualDragMesh);

        // Move the virtualObjectsPosition into the picked mesh's space if
        // needed
        _targetPosition.copyFrom(_virtualDragMesh->absolutePosition());
        if (pickedMesh->parent) {
          auto worldMat = *pickedMesh->parent()->getWorldMatrix();
          Vector3::TransformCoordinatesToRef(
            _targetPosition, Matrix::Invert(worldMat), _targetPosition);
        }

        if (!_moving) {
          _startingOrientation.copyFrom(
            *_virtualDragMesh->rotationQuaternion());
        }
        _moving = true;
      }
    }
  });

  Quaternion tmpQuaternion;
  // On every frame move towards target scaling to avoid jitter caused by vr
  // controllers
  _sceneRenderObserver = ownerNode->getScene()->onBeforeRenderObservable.add(
    [&](Scene* /*scene*/, EventState& /*es*/) {
      if (dragging && _moving && pickedMesh) {
        // Slowly move mesh to avoid jitter
        pickedMesh->position().addInPlace(
          _targetPosition.subtract(pickedMesh->position())
            .scale(dragDeltaRatio));

        // Get change in rotation
        tmpQuaternion.copyFrom(_startingOrientation);
        tmpQuaternion.x = -tmpQuaternion.x;
        tmpQuaternion.y = -tmpQuaternion.y;
        tmpQuaternion.z = -tmpQuaternion.z;
        _virtualDragMesh->rotationQuaternion()->multiplyToRef(tmpQuaternion,
                                                              tmpQuaternion);
        // Convert change in rotation to only y axis rotation
        Quaternion::RotationYawPitchRollToRef(
          tmpQuaternion.toEulerAngles("xyz").y, 0, 0, tmpQuaternion);
        tmpQuaternion.multiplyToRef(_startingOrientation, tmpQuaternion);
        // Slowly move mesh to avoid jitter
        auto oldParent = pickedMesh->parent();
        pickedMesh->setParent(nullptr);
        Quaternion::SlerpToRef(*pickedMesh->rotationQuaternion(), tmpQuaternion,
                               dragDeltaRatio,
                               *pickedMesh->rotationQuaternion());
        pickedMesh->setParent(oldParent);
      }
    });
}

void SixDofDragBehavior::detach()
{
  _scene->onPointerObservable.remove(_pointerObserver);
  _ownerNode->getScene()->onBeforeRenderObservable.remove(_sceneRenderObserver);
  _virtualOriginMesh->dispose();
  _virtualDragMesh->dispose();
}

} // end of namespace BABYLON
