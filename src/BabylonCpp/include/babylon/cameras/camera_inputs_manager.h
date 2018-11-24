#ifndef BABYLON_CAMERAS_CAMERA_INPUTS_MANAGER_H
#define BABYLON_CAMERAS_CAMERA_INPUTS_MANAGER_H

#include <functional>
#include <map>
#include <memory>
#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/cameras/icamera_input.h>

using json = nlohmann::json;

namespace BABYLON {

template <class TCamera>
class BABYLON_SHARED_EXPORT CameraInputsManager {

public:
  CameraInputsManager();
  CameraInputsManager(TCamera* iCamera);
  ~CameraInputsManager();

  /**
   * Add an input method to a camera.
   * @see http://doc.babylonjs.com/how_to/customizing_camera_inputs
   * @param input camera input method
   */
  void add(std::shared_ptr<ICameraInput<TCamera>>&& input);

  /**
   * @brief Remove a specific input method from a camera
   * example: camera.inputs.remove(camera.inputs.attached.mouse);
   * @param inputToRemove camera input method
   */
  void remove(ICameraInput<TCamera>* inputToRemove);

  void removeByType(const std::string& inputType);
  std::function<void()> _addCheckInputs(const std::function<void()>& fn);
  void attachInput(ICameraInput<TCamera>* input);
  void attachElement(ICanvas* canvas, bool noPreventDefault = false);
  void detachElement(ICanvas* canvas, bool disconnect = false);
  void rebuildInputCheck();
  void checkInputs();
  std::unordered_map<std::string, std::shared_ptr<ICameraInput<TCamera>>>&
  attached();

  /**
   * @brief Remove all attached input methods from a camera
   */
  void clear();

  json serialize(json& serializedCamera) const;
  void parse(const json& parsedCamera);

public:
  ICanvas* attachedElement;
  bool noPreventDefault;
  TCamera* camera;

private:
  std::unordered_map<std::string, std::shared_ptr<ICameraInput<TCamera>>>
    _attached;

}; // end of class CameraInputsManager<TCamera>

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_CAMERA_INPUTS_MANAGER_H
