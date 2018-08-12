#include <babylon/cameras/camera_inputs_manager.h>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/camera.h>
#include <babylon/core/json.h>
#include <babylon/core/logging.h>

namespace BABYLON {

template <class TCamera>
CameraInputsManager<TCamera>::CameraInputsManager()
    : attachedElement{nullptr}, camera{nullptr}
{
}

template <class TCamera>
CameraInputsManager<TCamera>::CameraInputsManager(TCamera* iCamera)
    : attachedElement{nullptr}, camera{iCamera}
{
}

template <class TCamera>
CameraInputsManager<TCamera>::~CameraInputsManager()
{
}

template <class TCamera>
void CameraInputsManager<TCamera>::add(
  shared_ptr_t<ICameraInput<TCamera>>&& input)
{
  const string_t type = input->getSimpleName();
  if (stl_util::contains(_attached, type)) {
    BABYLON_LOGF_WARN("CameraInputsManager",
                      "camera input of type %s already exists on camera",
                      type.c_str());
    return;
  }

  _attached[type] = ::std::move(input);

  _attached[type]->camera = camera;

  if (attachedElement) {
    _attached[type]->attachControl(attachedElement);
  }
}

template <class TCamera>
void CameraInputsManager<TCamera>::remove(ICameraInput<TCamera>* inputToRemove)
{
  for (auto& item : _attached) {
    auto& input = item.second;
    if (input.get() == inputToRemove) {
      input->detachControl(attachedElement);
      input->camera = nullptr;
      _attached.erase(item.first);
      rebuildInputCheck();
    }
  }
}

template <class TCamera>
void CameraInputsManager<TCamera>::removeByType(const string_t& inputType)
{
  for (auto& item : _attached) {
    auto& input = item.second;
    if (input->getClassName() == inputType) {
      input->detachControl(attachedElement);
      input->camera = nullptr;
      _attached.erase(item.first);
      rebuildInputCheck();
    }
  }
}

template <class TCamera>
::std::function<void()>
CameraInputsManager<TCamera>::_addCheckInputs(const ::std::function<void()>& fn)
{
  return [&]() { fn(); };
}

template <class TCamera>
void CameraInputsManager<TCamera>::attachInput(ICameraInput<TCamera>* input)
{
  if (attachedElement) {
    input->attachControl(attachedElement, noPreventDefault);
  }
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

  for (auto& item : _attached) {
    item.second->attachControl(canvas, noPreventDefault);
  }
}

template <class TCamera>
void CameraInputsManager<TCamera>::detachElement(ICanvas* canvas,
                                                 bool disconnect)
{
  if (attachedElement != canvas) {
    return;
  }

  for (auto& item : _attached) {
    item.second->detachControl(canvas);

    if (disconnect) {
      item.second->camera = nullptr;
    }
  }

  attachedElement = nullptr;
}

template <class TCamera>
void CameraInputsManager<TCamera>::rebuildInputCheck()
{
}

template <class TCamera>
void CameraInputsManager<TCamera>::checkInputs()
{
  for (auto& item : _attached) {
    auto& input = item.second;
    if (input->hasCheckInputs) {
      input->checkInputs();
    }
  }
}

template <class TCamera>
unordered_map_t<string_t, shared_ptr_t<ICameraInput<TCamera>>>&
CameraInputsManager<TCamera>::attached()
{
  return _attached;
}

template <class TCamera>
void CameraInputsManager<TCamera>::clear()
{
  if (attachedElement) {
    detachElement(attachedElement, true);
  }
  _attached.clear();
  attachedElement = nullptr;
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
template class CameraInputsManager<Camera>;
template class CameraInputsManager<FreeCamera>;

} // end of namespace BABYLON
