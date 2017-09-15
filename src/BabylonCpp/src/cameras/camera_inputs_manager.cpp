#include <babylon/cameras/camera_inputs_manager.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/core/logging.h>

namespace BABYLON {

template <class TCamera>
CameraInputsManager<TCamera>::CameraInputsManager(TCamera* _camera)
    : attachedElement{nullptr}, camera{_camera}
{
  checkInputs = []() {};
}

template <class TCamera>
CameraInputsManager<TCamera>::~CameraInputsManager()
{
}

template <class TCamera>
void CameraInputsManager<TCamera>::add(
  unique_ptr_t<ICameraInput<TCamera>>&& input)
{
  const string_t type = input->getSimpleName();
  if (stl_util::contains(attached, type)) {
    BABYLON_LOGF_WARN("CameraInputsManager",
                      "camera input of type %s already exists on camera",
                      type.c_str());
    return;
  }

  attached[type]                  = ::std::move(input);
  ICameraInput<TCamera>* inputPtr = attached[type].get();

  inputPtr->camera = camera;

  // for checkInputs, we are dynamically creating a function the goal is to
  // avoid the performance penalty of looping for inputs in the render loop
  if (inputPtr->hasCheckInputs) {
    checkInputs = _addCheckInputs([&inputPtr]() { inputPtr->checkInputs(); });
  }

  if (attachedElement) {
    inputPtr->attachControl(attachedElement);
  }
}

template <class TCamera>
void CameraInputsManager<TCamera>::remove(ICameraInput<TCamera>* inputToRemove)
{
  for (auto& item : attached) {
    auto& input = item.second;
    if (input.get() == inputToRemove) {
      input->detachControl(attachedElement);
      attached.erase(item.first);
      rebuildInputCheck();
    }
  }
}

template <class TCamera>
void CameraInputsManager<TCamera>::removeByType(const string_t& inputType)
{
  for (auto& item : attached) {
    auto& input = item.second;
    if (input->getClassName() == inputType) {
      input->detachControl(attachedElement);
      attached.erase(item.first);
      rebuildInputCheck();
    }
  }
}

template <class TCamera>
::std::function<void()> CameraInputsManager<TCamera>::_addCheckInputs(
  const ::std::function<void()>& /*fn*/)
{
  return [&]() {
    // checkInputs();
    // fn();
  };
}

template <class TCamera>
void CameraInputsManager<TCamera>::attachInput(ICameraInput<TCamera>* input)
{
  input->attachControl(attachedElement, noPreventDefault);
}

template <class TCamera>
void CameraInputsManager<TCamera>::attachElement(ICanvas* canvas,
                                                 bool _noPreventDefault)
{
  if (attachedElement) {
    return;
  }

  noPreventDefault = Camera::ForceAttachControlToAlwaysPreventDefault ?
                       false :
                       _noPreventDefault;
  attachedElement  = canvas;
  noPreventDefault = _noPreventDefault;

  for (auto& item : attached) {
    item.second->attachControl(canvas, noPreventDefault);
  }
}

template <class TCamera>
void CameraInputsManager<TCamera>::detachElement(ICanvas* canvas)
{
  if (attachedElement != canvas) {
    return;
  }

  for (auto& item : attached) {
    item.second->detachControl(canvas);
  }

  attachedElement = nullptr;
}

template <class TCamera>
void CameraInputsManager<TCamera>::rebuildInputCheck()
{
  checkInputs = []() {};

  for (auto& item : attached) {
    auto& input = item.second;
    if (input->hasCheckInputs) {
      checkInputs = _addCheckInputs([&input]() { input->checkInputs(); });
    }
  }
}

template <class TCamera>
void CameraInputsManager<TCamera>::clear()
{
  if (attachedElement) {
    detachElement(attachedElement);
  }
  attached.clear();
  attachedElement = nullptr;
  checkInputs     = []() {};
}

template <class TCamera>
Json::object CameraInputsManager<TCamera>::serialize(
  Json::object& /*serializedCamera*/) const
{
  return Json::object();
}

template <class TCamera>
void CameraInputsManager<TCamera>::parse(const Json::value& /*parsedCamera*/)
{
}

template class CameraInputsManager<ArcRotateCamera>;
template class CameraInputsManager<FreeCamera>;

} // end of namespace BABYLON
