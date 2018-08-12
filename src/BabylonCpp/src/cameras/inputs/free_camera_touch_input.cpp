#include <babylon/cameras/inputs/free_camera_touch_input.h>

namespace BABYLON {

FreeCameraTouchInput::FreeCameraTouchInput()
    : touchAngularSensibility{200000.f}
    , touchMoveSensibility{250.f}
    , _offsetX{nullopt_t}
    , _offsetY{nullopt_t}
    , _pointerInput{nullptr}
    , _observer{nullptr}
    , _onLostFocus{nullptr}
{
}

FreeCameraTouchInput::~FreeCameraTouchInput()
{
}

void FreeCameraTouchInput::attachControl(ICanvas* /*canvas*/,
                                         bool /*noPreventDefault*/)
{
}

void FreeCameraTouchInput::detachControl(ICanvas* /*canvas*/)
{
}

void FreeCameraTouchInput::checkInputs()
{
}

const char* FreeCameraTouchInput::getClassName() const
{
  return "FreeCameraTouchInput";
}

const char* FreeCameraTouchInput::getSimpleName() const
{
  return "touch";
}

} // end of namespace BABYLON
