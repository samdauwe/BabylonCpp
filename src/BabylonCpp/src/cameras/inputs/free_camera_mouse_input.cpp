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
    , _allowCameraRotation{true}
    , touchEnabled{iTouchEnabled}
    , _canvas{nullptr}
    , _engine{nullptr}
    , _pointerInput{nullptr}
    , _onMouseMove{nullptr}
    , _observer{nullptr}
    , previousPosition{std::nullopt}
    , _noPreventDefault{false}
{
}

FreeCameraMouseInput::~FreeCameraMouseInput() = default;

void FreeCameraMouseInput::attachControl(ICanvas* canvas, bool noPreventDefault)
{
  _canvas           = canvas;
  _engine           = camera->getEngine();
  _noPreventDefault = noPreventDefault;

  if (!_pointerInput) {
    _pointerInput = [this](PointerInfo* p, EventState&) -> void {
      auto& evt = p->pointerEvent;

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

        previousPosition = PositionCoord{
          evt.clientX, // x
          evt.clientY  // y
        };

        if (!_noPreventDefault) {
          evt.preventDefault();
          _canvas->focus();
        }
      }
      else if (p->type == PointerEventTypes::POINTERUP && srcElement) {
        srcElement->releasePointerCapture(evt.pointerId);

        previousPosition = std::nullopt;
        if (!_noPreventDefault) {
          evt.preventDefault();
        }
      }

      else if (p->type == PointerEventTypes::POINTERMOVE) {
        if (!previousPosition.has_value() && !_engine->isPointerLock) {
          return;
        }

        auto offsetX = static_cast<float>(evt.clientX - previousPosition->x);
        auto offsetY = static_cast<float>(evt.clientY - previousPosition->y);
        if (camera->getScene()->useRightHandedSystem()) {
          offsetX *= -1.f;
        }
        if (camera->parent
            && camera->parent()->_getWorldMatrixDeterminant() < 0.f) {
          offsetX *= -1.f;
        }

        if (_allowCameraRotation) {
          camera->cameraRotation->y += offsetX / angularSensibility;
          camera->cameraRotation->x += offsetY / angularSensibility;
        }

        PointerEvent pointerEvent;
        pointerEvent.offsetX = static_cast<int>(offsetX);
        pointerEvent.offsetY = static_cast<int>(offsetY);
        onPointerMovedObservable.notifyObservers(&pointerEvent);

        previousPosition = PositionCoord{
          evt.clientX, // x
          evt.clientY  // y
        };

        if (!_noPreventDefault) {
          evt.preventDefault();
        }
      }
    };
  }

  _onMouseMove = [this](MouseEvent& evt) -> void {
    if (!_engine->isPointerLock) {
      return;
    }

    if (_engine->isInVRExclusivePointerMode()) {
      return;
    }

    auto offsetX = static_cast<float>(evt.movementX);
    if (camera->getScene()->useRightHandedSystem()) {
      offsetX *= -1.f;
    }
    if (camera->parent
        && camera->parent()->_getWorldMatrixDeterminant() < 0.f) {
      offsetX *= -1.f;
    }
    camera->cameraRotation->y += offsetX / angularSensibility;

    auto offsetY = static_cast<float>(evt.movementY);
    camera->cameraRotation->x += offsetY / angularSensibility;

    previousPosition = std::nullopt;

    if (!_noPreventDefault) {
      evt.preventDefault();
    }
  };

  _observer = camera->getScene()->onPointerObservable.add(
    _pointerInput, static_cast<int>(PointerEventTypes::POINTERDOWN)
                     | static_cast<int>(PointerEventTypes::POINTERUP)
                     | static_cast<int>(PointerEventTypes::POINTERMOVE));
}

void FreeCameraMouseInput::onContextMenu(PointerEvent& evt)
{
  evt.preventDefault();
}

void FreeCameraMouseInput::detachControl(ICanvas* canvas)
{
  if (_observer && canvas) {
    camera->getScene()->onPointerObservable.remove(_observer);

    {
      onPointerMovedObservable.clear();
    }

    _observer        = nullptr;
    _onMouseMove     = nullptr;
    previousPosition = std::nullopt;
  }
}

void FreeCameraMouseInput::checkInputs()
{
}

std::string FreeCameraMouseInput::getClassName() const
{
  return "FreeCameraMouseInput";
}

std::string FreeCameraMouseInput::getSimpleName() const
{
  return "mouse";
}

} // end of namespace BABYLON
