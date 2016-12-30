#include <babylon/cameras/inputs/free_camera_mouse_input.h>

#include <babylon/engine/engine.h>
#include <babylon/engine/pointer_event_types.h>
#include <babylon/interfaces/icanvas.h>

namespace BABYLON {

FreeCameraMouseInput::FreeCameraMouseInput(bool iTouchEnabled)
    : buttons{{MouseButtonType::LEFT, MouseButtonType::MIDDLE,
               MouseButtonType::RIGHT}}
    , angularSensibility{2000.f}
    , touchEnabled{iTouchEnabled}
    , _canvas{nullptr}
    , _engine{nullptr}
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
    _pointerInput = [this](const PointerInfo& p, const EventState& /*s*/) {
      auto evt = p.pointerEvent;

      if (!touchEnabled && evt.pointerType == PointerType::TOUCH) {
        return;
      }

      if (p.type != PointerEventTypes::POINTERMOVE
          && evt.button == MouseButtonType::UNDEFINED) {
        return;
      }

      if (p.type == PointerEventTypes::POINTERDOWN) {
        if (evt.srcElement) {
          evt.srcElement->setPointerCapture(evt.pointerId);
        }

        _previousPosition.x      = evt.clientX;
        _previousPosition.y      = evt.clientY;
        _previousPositionDefined = true;

        if (!_noPreventDefault) {
          evt.preventDefault();
          _canvas->focus();
        }
      }
      else if (p.type == PointerEventTypes::POINTERUP) {
        if (evt.srcElement) {
          evt.srcElement->releasePointerCapture(evt.pointerId);
        }

        _previousPositionDefined = false;
        if (!_noPreventDefault) {
          evt.preventDefault();
        }
      }

      else if (p.type == PointerEventTypes::POINTERMOVE) {
        if (!_previousPositionDefined && !_engine->isPointerLock) {
          return;
        }

        float offsetX = static_cast<float>(evt.clientX - _previousPosition.x);
        float offsetY = static_cast<float>(evt.clientY - _previousPosition.y);

        if (camera->getScene()->useRightHandedSystem) {
          camera->cameraRotation->y -= offsetX / angularSensibility;
        }
        else {
          camera->cameraRotation->y += offsetX / angularSensibility;
        }

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

    float offsetX = static_cast<float>(evt.movementX);
    float offsetY = static_cast<float>(evt.movementY);

    if (camera->getScene()->useRightHandedSystem) {
      camera->cameraRotation->y -= offsetX / angularSensibility;
    }
    else {
      camera->cameraRotation->y += offsetX / angularSensibility;
    }

    camera->cameraRotation->x += offsetY / angularSensibility;

    _previousPositionDefined = false;

    if (!_noPreventDefault) {
      evt.preventDefault();
    }
  };

  _observer = camera->getScene()->onPointerObservable.add(
    _pointerInput, PointerEventTypes::POINTERDOWN | PointerEventTypes::POINTERUP
                     | PointerEventTypes::POINTERMOVE);
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

const char* FreeCameraMouseInput::getTypeName() const
{
  return "FreeCameraMouseInput";
}

const char* FreeCameraMouseInput::getSimpleName() const
{
  return "mouse";
}

} // end of namespace BABYLON
