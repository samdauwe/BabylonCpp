#include <babylon/cameras/camera_inputs_manager.h>

#include <nlohmann/json.hpp>

#include <babylon/babylon_stl_util.h>
#include <babylon/cameras/arc_rotate_camera.h>
#include <babylon/cameras/camera.h>
#include <babylon/cameras/fly_camera.h>
#include <babylon/cameras/follow_camera.h>
#include <babylon/core/logging.h>

namespace BABYLON {

template <class TCamera>
CameraInputsManager<TCamera>::CameraInputsManager() : CameraInputsManager{nullptr}
{
}

template <class TCamera>
CameraInputsManager<TCamera>::CameraInputsManager(TCamera* iCamera)
    : attachedToElement{false}, camera{iCamera}
{
  checkInputs = [this]() -> void {
    for (const auto& item : attached) {
      const auto& input = item.second;
      if (input->hasCheckInputs) {
        input->checkInputs();
      }
    }
  };
}

template <class TCamera>
CameraInputsManager<TCamera>::~CameraInputsManager() = default;

template <class TCamera>
void CameraInputsManager<TCamera>::add(std::shared_ptr<ICameraInput<TCamera>>&& input)
{
  const std::string type = input->getSimpleName();
  if (stl_util::contains(attached, type)) {
    BABYLON_LOGF_WARN("CameraInputsManager", "camera input of type %s already exists on camera",
                      type.c_str())
    return;
  }

  attached[type] = std::move(input);

  attached[type]->camera = camera;

  if (attachedToElement) {
    attached[type]->attachControl();
  }
}

template <class TCamera>
void CameraInputsManager<TCamera>::remove(ICameraInput<TCamera>* inputToRemove)
{
  for (const auto& item : attached) {
    const auto& input = item.second;
    if (input.get() == inputToRemove) {
      input->detachControl();
      input->camera = nullptr;
      attached.erase(item.first);
      rebuildInputCheck();
    }
  }
}

template <class TCamera>
void CameraInputsManager<TCamera>::removeByType(const std::string& inputType)
{
  for (const auto& item : attached) {
    const auto& input = item.second;
    if (input->getClassName() == inputType) {
      input->detachControl();
      input->camera = nullptr;
      attached.erase(item.first);
      rebuildInputCheck();
    }
  }
}

template <class TCamera>
std::function<void()> CameraInputsManager<TCamera>::_addCheckInputs(const std::function<void()>& fn)
{
  return [&]() { fn(); };
}

template <class TCamera>
void CameraInputsManager<TCamera>::attachInput(ICameraInput<TCamera>* input)
{
  if (attachedToElement) {
    input->attachControl(noPreventDefault);
  }
}

template <class TCamera>
void CameraInputsManager<TCamera>::attachElement(bool _noPreventDefault)
{
  if (attachedToElement) {
    return;
  }

  noPreventDefault  = Camera::ForceAttachControlToAlwaysPreventDefault ? false : _noPreventDefault;
  attachedToElement = true;
  noPreventDefault  = _noPreventDefault;

  for (auto& item : attached) {
    item.second->attachControl(noPreventDefault);
  }
}

template <class TCamera>
void CameraInputsManager<TCamera>::detachElement(bool disconnect)
{
  for (const auto& item : attached) {
    item.second->detachControl();

    if (disconnect) {
      item.second->camera = nullptr;
    }
  }

  attachedToElement = false;
}

template <class TCamera>
void CameraInputsManager<TCamera>::rebuildInputCheck()
{
  checkInputs = []() -> void {};
}

template <class TCamera>
void CameraInputsManager<TCamera>::clear()
{
  if (attachedToElement) {
    detachElement(true);
  }
  attached.clear();
  attachedToElement = false;
  checkInputs       = []() -> void {};
}

template <class TCamera>
json CameraInputsManager<TCamera>::serialize(json& /*serializedCamera*/) const
{
  return nullptr;
}

template <class TCamera>
void CameraInputsManager<TCamera>::parse(const json& /*parsedCamera*/)
{
}

template class CameraInputsManager<ArcRotateCamera>;
template class CameraInputsManager<Camera>;
template class CameraInputsManager<FlyCamera>;
template class CameraInputsManager<FollowCamera>;
template class CameraInputsManager<FreeCamera>;

} // end of namespace BABYLON
