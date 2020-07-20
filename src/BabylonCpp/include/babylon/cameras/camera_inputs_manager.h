#ifndef BABYLON_CAMERAS_CAMERA_INPUTS_MANAGER_H
#define BABYLON_CAMERAS_CAMERA_INPUTS_MANAGER_H

#include <functional>
#include <memory>
#include <unordered_map>

#include <nlohmann/json_fwd.hpp>

#include <babylon/babylon_api.h>
#include <babylon/cameras/icamera_input.h>

using json = nlohmann::json;

namespace BABYLON {

/**
 * @brief This represents the input manager used within a camera.
 * It helps dealing with all the different kind of input attached to a camera.
 * @see https://doc.babylonjs.com/how_to/customizing_camera_inputs
 */
template <class TCamera>
class BABYLON_SHARED_EXPORT CameraInputsManager {

public:
  /**
   * @brief Instantiate a new Camera Input Manager.
   * @param camera Defines the camera the input manager blongs to
   */
  CameraInputsManager();
  CameraInputsManager(TCamera* iCamera);
  ~CameraInputsManager(); // = default

  /**
   * @brief Add an input method to a camera.
   * @see https://doc.babylonjs.com/how_to/customizing_camera_inputs
   * @param input camera input method
   */
  void add(std::shared_ptr<ICameraInput<TCamera>>&& input);

  /**
   * @brief Remove a specific input method from a camera.
   * example: camera.inputs.remove(camera.inputs.attached.mouse);
   * @param inputToRemove camera input method
   */
  void remove(ICameraInput<TCamera>* inputToRemove);

  /**
   * @brief Remove a specific input type from a camera.
   * example: camera.inputs.remove("ArcRotateCameraGamepadInput");
   * @param inputType the type of the input to remove
   */
  void removeByType(const std::string& inputType);

  /**
   * @brief Attach the input controls to the currently attached dom element to
   * listen the events from.
   * @param input Defines the input to attach
   */
  void attachInput(ICameraInput<TCamera>* input);

  /**
   * @brief Attach the current manager inputs controls to a specific dom element
   * to listen the events from.
   * @param element Defines the dom element to collect the events from
   * @param noPreventDefault Defines whether event caught by the controls should
   * call preventdefault()
   * (https://developer.mozilla.org/en-US/docs/Web/API/Event/preventDefault)
   */
  void attachElement(ICanvas* canvas, bool noPreventDefault = false);

  /**
   * @brief Detach the current manager inputs controls from a specific dom
   * element.
   * @param element Defines the dom element to collect the events from
   * @param disconnect Defines whether the input should be removed from the
   * current list of attached inputs
   */
  void detachElement(ICanvas* canvas, bool disconnect = false);

  /**
   * @brief Rebuild the dynamic inputCheck function from the current list of
   * defined inputs in the manager.
   */
  void rebuildInputCheck();

  /**
   * @brief Remove all attached input methods from a camera
   */
  void clear();

  /**
   * @brief Serialize the current input manager attached to a camera.
   * This ensures than once parsed,
   * the input associated to the camera will be identical to the current ones
   * @param serializedCamera Defines the camera serialization JSON the input
   * serialization should write to
   */
  json serialize(json& serializedCamera) const;

  /**
   * @brief Parses an input manager serialized JSON to restore the previous list
   * of inputs and states associated to a camera.
   * @param parsedCamera Defines the JSON to parse
   */
  void parse(const json& parsedCamera);

private:
  std::function<void()> _addCheckInputs(const std::function<void()>& fn);

public:
  /**
   * Defines the list of inputs attahed to the camera.
   */
  std::unordered_map<std::string, std::shared_ptr<ICameraInput<TCamera>>> attached;

  /**
   * Defines the dom element the camera is collecting inputs from.
   * This is null if the controls have not been attached.
   */
  ICanvas* attachedElement;

  /**
   * Defines whether event caught by the controls should call preventdefault()
   * (https://developer.mozilla.org/en-US/docs/Web/API/Event/preventDefault)
   */
  bool noPreventDefault;

  /**
   * Defined the camera the input manager belongs to.
   */
  TCamera* camera;

  /**
   * Update the current camera state depending on the inputs that have been used
   * this frame. This is a dynamically created lambda to avoid the performance
   * penalty of looping for inputs in the render loop.
   */
  std::function<void()> checkInputs;

}; // end of class CameraInputsManager<TCamera>

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_CAMERA_INPUTS_MANAGER_H
