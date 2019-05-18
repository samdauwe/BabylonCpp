#include <babylon/cameras/inputs/free_camera_mouse_input.h>

#include <babylon/engines/engine.h>
#include <babylon/engines/scene.h>
#include <babylon/events/pointer_event_types.h>
#include <babylon/interfaces/icanvas.h>

namespace BABYLON {

FreeCameraMouseInput::FreeCameraMouseInput(bool iTouchEnabled)
    : buttons{{MouseButtonType::LEFT, MouseButtonType::MIDDLE,
               MouseButtonType::RIGHT}}
    , angularSensibility{2000.f}
    , touchEnabled{iTouchEnabled}
    , _canvas{nullptr}
    , _engine{nullptr}
    , _pointerInput{nullptr}
    , _onMouseMove{nullptr}
    , _observer{nullptr}
    , _previousPositionDefined{false}
    , _noPreventDefault{false}
{
}

FreeCameraMouseInput::~FreeCameraMouseInput()
{
}

void FreeCameraMouseInput::attachControl(ICanvas* canvas, bool noPreventDefault)
{
  _canvas           = canvas;
  _engine           = camera->getEngine();
  _noPreventDefault = noPreventDefault;

  if (!_pointerInput) {
    _pointerInput = [this](PointerInfo* p, EventState&) {
      auto evt = p->pointerEvent;

      if (_engine->isInVRExclusivePointerMode()) {
        return;
      }

      if (!touchEnabled && evt.pointerType == PointerType::TOUCH) {
        return;
      }

      if (p->type != PointerEventTypes::POINTERMOVE
          && evt.button == MouseButtonType::UNDEFINED) {
        return;
      }

      auto srcElement = evt.srcElement;

      if (p->type == PointerEventTypes::POINTERDOWN && srcElement) {
        if (evt.srcElement) {
          srcElement->setPointerCapture(evt.pointerId);
        }

        _previousPosition.x      = evt.clientX;
        _previousPosition.y      = evt.clientY;
        _previousPositionDefined = true;

        if (!_noPreventDefault) {
          evt.preventDefault();
          _canvas->focus();
        }
      }
      else if (p->type == PointerEventTypes::POINTERUP && srcElement) {
        srcElement->releasePointerCapture(evt.pointerId);

        _previousPositionDefined = false;
        if (!_noPreventDefault) {
          evt.preventDefault();
        }
      }

      else if (p->type == PointerEventTypes::POINTERMOVE) {
        if (!_previousPositionDefined && !_engine->isPointerLock) {
          return;
        }

        float offsetX = static_cast<float>(evt.clientX - _previousPosition.x);
        if (camera->getScene()->useRightHandedSystem()) {
          offsetX *= -1.f;
        }
        if (camera->parent
            && camera->parent()->_getWorldMatrixDeterminant() < 0.f) {
          offsetX *= -1.f;
        }
        camera->cameraRotation->y += offsetX / angularSensibility;

        float offsetY = static_cast<float>(evt.clientY - _previousPosition.y);
        camera->cameraRotation->x += offsetY / angularSensibility;

        _previousPosition.x      = evt.clientX;
        _previousPosition.y      = evt.clientY;
        _previousPositionDefined = true;

        if (!_noPreventDefault) {
          evt.preventDefault();
        }
      }
    };
  }

  _onMouseMove = [this](MouseEvent& evt) {
    if (!_engine->isPointerLock) {
      return;
    }

    if (_engine->isInVRExclusivePointerMode()) {
      return;
    }

    float offsetX = static_cast<float>(evt.movementX);
    if (camera->getScene()->useRightHandedSystem()) {
      offsetX *= -1.f;
    }
    if (camera->parent
        && camera->parent()->_getWorldMatrixDeterminant() < 0.f) {
      offsetX *= -1.f;
    }
    camera->cameraRotation->y += offsetX / angularSensibility;

    float offsetY = static_cast<float>(evt.movementY);
    camera->cameraRotation->x += offsetY / angularSensibility;

    _previousPositionDefined = false;

    if (!_noPreventDefault) {
      evt.preventDefault();
    }
  };

  _observer = camera->getScene()->onPointerObservable.add(
    _pointerInput, static_cast<int>(PointerEventTypes::POINTERDOWN)
                     | static_cast<int>(PointerEventTypes::POINTERUP)
                     | static_cast<int>(PointerEventTypes::POINTERMOVE));
}

void FreeCameraMouseInput::detachControl(ICanvas* canvas)
{
  if (_observer && canvas) {
    camera->getScene()->onPointerObservable.remove(_observer);
    _observer                = nullptr;
    _onMouseMove             = nullptr;
    _previousPositionDefined = false;
  }
}

void FreeCameraMouseInput::checkInputs()
{
}

const char* FreeCameraMouseInput::getClassName() const
{
  return "FreeCameraMouseInput";
}

const char* FreeCameraMouseInput::getSimpleName() const
{
  return "mouse";
}

} // end of namespace BABYLON
