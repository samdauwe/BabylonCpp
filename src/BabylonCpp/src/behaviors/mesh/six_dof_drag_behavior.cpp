#include <babylon/behaviors/mesh/six_dof_drag_behavior.h>

#include <babylon/cameras/camera.h>
#include <babylon/engine/engine.h>
#include <babylon/engine/scene.h>
#include <babylon/mesh/mesh.h>

namespace BABYLON {

std::unique_ptr<Scene> SixDofDragBehavior::_virtualScene = nullptr;

SixDofDragBehavior::SixDofDragBehavior()
    : dragging{false}
    , dragDeltaRatio{0.2f}
    , currentDraggingPointerID{-1}
    , detachCameraControls{true}
    , _ownerNode{nullptr}
    , _sceneRenderObserver{nullptr}
    , _scene{nullptr}
    , _targetPosition{Vector3(0.f, 0.f, 0.f)}
    , _virtualOriginMesh{nullptr}
    , _virtualDragMesh{nullptr}
    , _pointerObserver{nullptr}
    , _moving{false}
    , zDragFactor{3.f}
{
}

SixDofDragBehavior::~SixDofDragBehavior()
{
}

const std::string SixDofDragBehavior::name() const
{
  return "SixDofDrag";
}

void SixDofDragBehavior::init()
{
}

void SixDofDragBehavior::attach(const MeshPtr& ownerNode)
{
  _ownerNode = ownerNode;
  _scene     = _ownerNode->getScene();
  if (!SixDofDragBehavior::_virtualScene) {
    SixDofDragBehavior::_virtualScene = Scene::New(_scene->getEngine());
    _scene->getEngine()->scenes.pop_back();
  }

  AbstractMeshPtr pickedMesh = nullptr;
  Vector3 lastSixDofOriginPosition(0, 0, 0);

  // Setup virtual meshes to be used for dragging without dirtying the existing
  // scene
  _virtualOriginMesh
    = AbstractMesh::New("", SixDofDragBehavior::_virtualScene.get());
  _virtualOriginMesh->rotationQuaternion = Quaternion();
  _virtualDragMesh
    = AbstractMesh::New("", SixDofDragBehavior::_virtualScene.get());
  _virtualDragMesh->rotationQuaternion = Quaternion();

  const auto pickPredicate = [this](const AbstractMeshPtr& m) {
    return _ownerNode == m || m->isDescendantOf(_ownerNode.get());
  };

  ICanvas* attachedElement = nullptr;
  _pointerObserver         = _scene->onPointerObservable.add([&](
                                                       PointerInfo* pointerInfo,
                                                       EventState& /*es*/) {
    if (pointerInfo->type == PointerEventTypes::POINTERDOWN) {
      if (!dragging && pointerInfo->pickInfo.hit
          && pointerInfo->pickInfo.pickedMesh && pointerInfo->pickInfo.ray
          && pickPredicate(pointerInfo->pickInfo.pickedMesh)) {
        if (_scene->activeCamera
            && _scene->activeCamera->cameraRigMode == Camera::RIG_MODE_NONE()) {
          auto ray = *pointerInfo->pickInfo.ray;
          ray.origin.copyFrom(_scene->activeCamera->position);
          pointerInfo->pickInfo.ray = ray;
        }

        pickedMesh = _ownerNode;
        lastSixDofOriginPosition.copyFrom((*pointerInfo->pickInfo.ray).origin);

        // Set position and orientation of the controller
        _virtualOriginMesh->position().copyFrom(
          (*pointerInfo->pickInfo.ray).origin);
        _virtualOriginMesh->lookAt(
          (*pointerInfo->pickInfo.ray)
            .origin.subtract((*pointerInfo->pickInfo.ray).direction));

        // Attach the virtual drag mesh to the virtual origin mesh so it can be
        // dragged
        _virtualOriginMesh->removeChild(*_virtualDragMesh);
        _virtualDragMesh->position().copyFrom(pickedMesh->absolutePosition());
        if (!pickedMesh->rotationQuaternion()) {
          pickedMesh->rotationQuaternion = Quaternion::RotationYawPitchRoll(
            pickedMesh->rotation().y, pickedMesh->rotation().x,
            pickedMesh->rotation().z);
        }
        auto oldParent = pickedMesh->parent();
        pickedMesh->setParent(nullptr);
        _virtualDragMesh->rotationQuaternion()->copyFrom(
          *pickedMesh->rotationQuaternion());
        pickedMesh->setParent(oldParent);
        _virtualOriginMesh->addChild(*_virtualDragMesh);

        // Update state
        _targetPosition.copyFrom(_virtualDragMesh->absolutePosition());
        dragging                 = true;
        currentDraggingPointerID = (pointerInfo->pointerEvent).pointerId;

        // Detatch camera controls
        if (detachCameraControls && _scene->activeCamera
            && !_scene->activeCamera->leftCamera()) {
          if (_scene->activeCamera->inputs.attachedElement) {
            attachedElement = _scene->activeCamera->inputs.attachedElement;
            _scene->activeCamera->detachControl(
              _scene->activeCamera->inputs.attachedElement);
          }
          else {
            attachedElement = nullptr;
          }
        }
      }
    }
    else if (pointerInfo->type == PointerEventTypes::POINTERUP) {
      if (currentDraggingPointerID == (pointerInfo->pointerEvent).pointerId) {
        dragging                 = false;
        _moving                  = false;
        currentDraggingPointerID = -1;
        pickedMesh               = nullptr;
        _virtualOriginMesh->removeChild(*_virtualDragMesh);

        // Reattach camera controls
        if (detachCameraControls && attachedElement && _scene->activeCamera
            && !_scene->activeCamera->leftCamera()) {
          _scene->activeCamera->attachControl(attachedElement, true);
        }
      }
    }
    else if (pointerInfo->type == PointerEventTypes::POINTERMOVE) {
      if (currentDraggingPointerID == (pointerInfo->pointerEvent).pointerId
          && dragging && pointerInfo->pickInfo.ray && pickedMesh) {
        auto _zDragFactor = zDragFactor;
        if (_scene->activeCamera
            && _scene->activeCamera->cameraRigMode == Camera::RIG_MODE_NONE()) {
          auto ray = *pointerInfo->pickInfo.ray;
          ray.origin.copyFrom(_scene->activeCamera->position);
          pointerInfo->pickInfo.ray = ray;
          _zDragFactor              = 0.f;
        }

        // Calculate controller drag distance in controller space
        auto originDragDifference
          = (*pointerInfo->pickInfo.ray)
              .origin.subtract(lastSixDofOriginPosition);
        lastSixDofOriginPosition.copyFrom((*pointerInfo->pickInfo.ray).origin);

        auto localOriginDragDifference = -Vector3::Dot(
          originDragDifference, (*pointerInfo->pickInfo.ray).direction);

        _virtualOriginMesh->addChild(*_virtualDragMesh);
        // Determine how much the controller moved to/away towards the dragged
        // object and use this to move the object further when its further away
        _virtualDragMesh->position().z
          -= _virtualDragMesh->position().z < 1 ?
               localOriginDragDifference * _zDragFactor :
               localOriginDragDifference * _zDragFactor
                 * _virtualDragMesh->position().z;
        if (_virtualDragMesh->position().z < 0) {
          _virtualDragMesh->position().z = 0;
        }

        // Update the controller position
        _virtualOriginMesh->position().copyFrom(
          (*pointerInfo->pickInfo.ray).origin);
        _virtualOriginMesh->lookAt(
          (*pointerInfo->pickInfo.ray)
            .origin.subtract((*pointerInfo->pickInfo.ray).direction));
        _virtualOriginMesh->removeChild(*_virtualDragMesh);

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
  if (_scene) {
    _scene->onPointerObservable.remove(_pointerObserver);
  }
  if (_ownerNode) {
    _ownerNode->getScene()->onBeforeRenderObservable.remove(
      _sceneRenderObserver);
  }
  if (_virtualOriginMesh) {
    _virtualOriginMesh->dispose();
  }
  if (_virtualDragMesh) {
    _virtualDragMesh->dispose();
  }
}

} // end of namespace BABYLON
