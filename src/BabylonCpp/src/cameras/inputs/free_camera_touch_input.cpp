#include <babylon/cameras/inputs/free_camera_touch_input.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/engines/scene.h>

namespace BABYLON {

FreeCameraTouchInput::FreeCameraTouchInput()
    : touchAngularSensibility{200000.f}
    , touchMoveSensibility{250.f}
    , _offsetX{std::nullopt}
    , _offsetY{std::nullopt}
    , _pointerInput{nullptr}
    , _observer{nullptr}
    , _onLostFocus{nullptr}
    , previousPosition{std::nullopt}
    , noPreventDefault{false}
{
}

FreeCameraTouchInput::~FreeCameraTouchInput() = default;

void FreeCameraTouchInput::attachControl(ICanvas* /*canvas*/, bool iNoPreventDefault)
{
  noPreventDefault = iNoPreventDefault;
  previousPosition = std::nullopt;

  if (_pointerInput == nullptr) {
    _onLostFocus = [this](FocusEvent & /*e*/) -> void {
      _offsetX = std::nullopt;
      _offsetY = std::nullopt;
    };

    _pointerInput = [this](PointerInfo* p, EventState & /*es*/) -> void {
      auto& evt = p->pointerEvent;

      if (evt.pointerType == PointerType::MOUSE) {
        return;
      }

      if (p->type == PointerEventTypes::POINTERDOWN) {

        if (!noPreventDefault) {
          evt.preventDefault();
        }

        _pointerPressed.emplace_back(evt.pointerId);

        if (_pointerPressed.size() != 1) {
          return;
        }

        previousPosition = PositionCoord{
          evt.clientX, // x
          evt.clientY  // y
        };
      }

      else if (p->type == PointerEventTypes::POINTERUP) {
        if (!noPreventDefault) {
          evt.preventDefault();
        }

        auto it    = std::find(_pointerPressed.begin(), _pointerPressed.end(), evt.pointerId);
        auto index = stl_util::index_of(_pointerPressed, evt.pointerId);

        if (index == -1) {
          return;
        }
        _pointerPressed.erase(it);

        if (index != 0) {
          return;
        }
        previousPosition = std::nullopt;
        _offsetX         = std::nullopt;
        _offsetY         = std::nullopt;
      }

      else if (p->type == PointerEventTypes::POINTERMOVE) {
        if (!noPreventDefault) {
          evt.preventDefault();
        }

        if (!previousPosition) {
          return;
        }

        auto index = stl_util::index_of(_pointerPressed, evt.pointerId);

        if (index != 0) {
          return;
        }

        _offsetX = static_cast<float>(evt.clientX) - previousPosition->x;
        _offsetY = -(static_cast<float>(evt.clientY) - previousPosition->y);
      }
    };
  }

  _observer = camera->getScene()->onPointerObservable.add(
    _pointerInput, static_cast<int>(PointerEventTypes::POINTERDOWN)
                     | static_cast<int>(PointerEventTypes::POINTERUP)
                     | static_cast<int>(PointerEventTypes::POINTERMOVE));

  if (_onLostFocus) {
    // canvas->addEventListener("blur", _onLostFocus);
  }
}

void FreeCameraTouchInput::detachControl(ICanvas* canvas)
{
  if (_pointerInput && canvas) {
    if (_observer) {
      camera->getScene()->onPointerObservable.remove(_observer);
      _observer = nullptr;
    }

    if (_onLostFocus) {
      // canvas->removeEventListener("blur", _onLostFocus);
      _onLostFocus = nullptr;
    }
    _pointerPressed.clear();
    _offsetX = std::nullopt;
    _offsetY = std::nullopt;
  }
}

void FreeCameraTouchInput::checkInputs()
{
  if (_offsetX.has_value() && _offsetY.has_value()) {
    camera->cameraRotation->y += _offsetX.value() / touchAngularSensibility;

    if (_pointerPressed.size() > 1) {
      camera->cameraRotation->x += -_offsetY.value() / touchAngularSensibility;
    }
    else {
      auto speed = camera->_computeLocalCameraSpeed();
      Vector3 direction(0.f, 0.f, speed * _offsetY.value() / touchMoveSensibility);

      Matrix::RotationYawPitchRollToRef(camera->rotation().y, camera->rotation().x, 0.f,
                                        camera->_cameraRotationMatrix);
      camera->cameraDirection->addInPlace(
        Vector3::TransformCoordinates(direction, camera->_cameraRotationMatrix));
    }
  }
}

std::string FreeCameraTouchInput::getClassName() const
{
  return "FreeCameraTouchInput";
}

std::string FreeCameraTouchInput::getSimpleName() const
{
  return "touch";
}

} // end of namespace BABYLON
