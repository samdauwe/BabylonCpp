#include <babylon/cameras/inputs/fly_camera_mouse_input.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/interfaces/icanvas.h>
#include <babylon/maths/axis.h>

namespace BABYLON {

FlyCameraMouseInput::FlyCameraMouseInput(bool iTouchEnabled)
    : touchEnabled{iTouchEnabled}
    , buttons{{MouseButtonType::LEFT, MouseButtonType::MIDDLE, MouseButtonType::RIGHT}}
    , buttonsYaw{{-1, 0, 1}}
    , buttonsPitch{{-1, 0, 1}}
    , buttonsRoll{2}
    , activeButton{-1}
    , angularSensibility{1000.f}
    , _mousemoveCallback{nullptr}
    , _observer{nullptr}
    , _rollObserver{nullptr}
    , previousPosition{std::nullopt}
    , _noPreventDefault{std::nullopt}
    , _canvas{nullptr}
{
}

FlyCameraMouseInput::~FlyCameraMouseInput() = default;

void FlyCameraMouseInput::attachControl(ICanvas* canvas, bool noPreventDefault)
{
  _canvas           = canvas;
  _noPreventDefault = noPreventDefault;

  _observer = camera->getScene()->onPointerObservable.add(
    [this](PointerInfo* p, EventState& s) -> void { _pointerInput(p, s); },
    static_cast<int>(PointerEventTypes::POINTERDOWN)
      | static_cast<int>(PointerEventTypes::POINTERUP)
      | static_cast<int>(PointerEventTypes::POINTERMOVE));

  // Correct Roll by rate, if enabled.
  _rollObserver = camera->getScene()->onBeforeRenderObservable.add(
    [this](Scene* /*scene*/, EventState & /*es*/) -> void {
      if (camera->rollCorrect != 0.f) {
        camera->restoreRoll(camera->rollCorrect);
      }
    });

  // Helper function to keep 'this'.
  _mousemoveCallback = [this](MouseEvent& e) -> void { _onMouseMove(e); };
  // canvas->addEventListener("mousemove", _mousemoveCallback, false);
}

void FlyCameraMouseInput::detachControl(ICanvas* canvas)
{
  if (_observer && canvas) {
    camera->getScene()->onPointerObservable.remove(_observer);

    camera->getScene()->onBeforeRenderObservable.remove(_rollObserver);

    if (_mousemoveCallback) {
      // canvas->removeEventListener("mousemove", _mousemoveCallback);
    }

    _observer         = nullptr;
    _rollObserver     = nullptr;
    previousPosition  = std::nullopt;
    _noPreventDefault = std::nullopt;
  }
}

void FlyCameraMouseInput::checkInputs()
{
}

std::string FlyCameraMouseInput::getClassName() const
{
  return "FlyCameraMouseInput";
}

std::string FlyCameraMouseInput::getSimpleName() const
{
  return "mouse";
}

void FlyCameraMouseInput::_pointerInput(PointerInfo* p, EventState& /*s*/)
{
  auto& e = p->pointerEvent;

  auto engine = camera->getEngine();

  if (engine->isInVRExclusivePointerMode()) {
    return;
  }

  if (!touchEnabled && e.pointerType == PointerType::TOUCH) {
    return;
  }

  // Mouse is moved but an unknown mouse button is pressed.
  if (p->type != PointerEventTypes::POINTERMOVE
      && (std::find(buttons.begin(), buttons.end(), e.button) == buttons.end())) {
    return;
  }

  auto srcElement = e.srcElement;

  // Mouse down.
  if (p->type == PointerEventTypes::POINTERDOWN && srcElement) {
    if (srcElement) {
      srcElement->setPointerCapture(e.pointerId);
    }

    previousPosition = PositionCoord{
      e.clientX, // x
      e.clientY  // y
    };

    if (!_noPreventDefault) {
      e.preventDefault();
      _canvas->focus();
    }
  }
  else {
    // Mouse up.
    if (p->type == PointerEventTypes::POINTERUP && srcElement) {
      srcElement->releasePointerCapture(e.pointerId);

      activeButton = -1;

      previousPosition = std::nullopt;
      if (!_noPreventDefault) {
        e.preventDefault();
      }
    }
    else {
      // Mouse move.
      if (p->type == PointerEventTypes::POINTERMOVE) {
        if (!previousPosition.has_value() && !engine->isPointerLock) {
          return;
        }

        auto offsetX = static_cast<float>(e.clientX - previousPosition->x);
        auto offsetY = static_cast<float>(e.clientY - previousPosition->y);

        rotateCamera(offsetX, offsetY);

        previousPosition = PositionCoord{
          e.clientX, // x
          e.clientY  // y
        };

        if (!_noPreventDefault) {
          e.preventDefault();
        }
      }
    }
  }
}

void FlyCameraMouseInput::_onMouseMove(MouseEvent& e)
{
  auto engine = camera->getEngine();

  if (!engine->isPointerLock || engine->isInVRExclusivePointerMode()) {
    return;
  }

  auto offsetX = static_cast<float>(e.movementX);
  auto offsetY = static_cast<float>(e.movementY);

  rotateCamera(offsetX, offsetY);

  previousPosition = std::nullopt;

  if (!_noPreventDefault) {
    e.preventDefault();
  }
}

void FlyCameraMouseInput::rotateCamera(float offsetX, float offsetY)
{
  auto scene = camera->getScene();

  if (scene->useRightHandedSystem()) {
    offsetX *= -1.f;
  }
  if (camera->parent && camera->parent()->_getWorldMatrixDeterminant() < 0.f) {
    offsetX *= -1.f;
  }

  auto x = offsetX / angularSensibility;
  auto y = offsetY / angularSensibility;

  // Initialize to current rotation.
  auto currentRotation = Quaternion::RotationYawPitchRoll(camera->rotation().y, //
                                                          camera->rotation().x, //
                                                          camera->rotation().z  //
  );
  Quaternion rotationChange;

  // Pitch.
  if (std::find(buttonsPitch.begin(), buttonsPitch.end(), activeButton) != buttonsPitch.end()) {
    // Apply change in Radians to vector Angle.
    auto rotationAxis = Axis::X();
    rotationChange    = Quaternion::RotationAxis(rotationAxis, y);
    // Apply Pitch to quaternion.
    currentRotation.multiplyInPlace(rotationChange);
  }

  // Yaw.
  if (std::find(buttonsYaw.begin(), buttonsYaw.end(), activeButton) != buttonsYaw.end()) {
    // Apply change in Radians to vector Angle.
    auto rotationAxis = Axis::Y();
    rotationChange    = Quaternion::RotationAxis(rotationAxis, x);
    // Apply Yaw to quaternion.
    currentRotation.multiplyInPlace(rotationChange);

    // Add Roll, if banked turning is enabled, within Roll limit.
    auto limit
      = (camera->bankedTurnLimit) + camera->_trackRoll; // Defaults to 90° plus manual roll.
    if (camera->bankedTurn && -limit < camera->rotation().z && camera->rotation().z < limit) {
      auto bankingDelta = camera->bankedTurnMultiplier * -x;
      // Apply change in Radians to vector Angle.
      auto _rotationAxis = Axis::Z();
      rotationChange     = Quaternion::RotationAxis(_rotationAxis, bankingDelta);
      // Apply Yaw to quaternion.
      currentRotation.multiplyInPlace(rotationChange);
    }
  }

  // Roll.
  if (std::find(buttonsRoll.begin(), buttonsRoll.end(), activeButton) != buttonsRoll.end()) {
    // Apply change in Radians to vector Angle.
    auto rotationAxis = Axis::Z();
    rotationChange    = Quaternion::RotationAxis(rotationAxis, -x);
    // Track Rolling.
    camera->_trackRoll -= x;
    // Apply Pitch to quaternion.
    currentRotation.multiplyInPlace(rotationChange);
  }

  // Apply rotationQuaternion to Euler camera.rotation.
  currentRotation.toEulerAnglesToRef(camera->rotation());
}

} // end of namespace BABYLON
