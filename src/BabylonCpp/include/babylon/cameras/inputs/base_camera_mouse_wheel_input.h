#ifndef BABYLON_CAMERAS_INPUTS_BASE_CAMERA_MOUSE_WHEEL_INPUT_H
#define BABYLON_CAMERAS_INPUTS_BASE_CAMERA_MOUSE_WHEEL_INPUT_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/babylon_fwd.h>
#include <babylon/cameras/icamera_input.h>
#include <babylon/core/structs.h>
#include <babylon/events/pointer_info.h>
#include <babylon/misc/observable.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

FWD_CLASS_SPTR(Camera)

/**
 * @brief Base class for mouse wheel input..
 * See FollowCameraMouseWheelInput in src/Cameras/Inputs/freeCameraMouseWheelInput.ts
 * for example usage.
 */
class BABYLON_SHARED_EXPORT BaseCameraMouseWheelInput {

public:
  /**
   * @brief Listen to mouse wheel events to control the camera.
   * @see https://doc.babylonjs.com/how_to/customizing_camera_inputs
   */
  BaseCameraMouseWheelInput();
  ~BaseCameraMouseWheelInput(); // = default

  /**
   * @brief Attach the mouse control to the HTML DOM element.
   * @param noPreventDefault Defines whether events caught by the controls should call
   * preventdefault().
   */
  void attachControl(bool noPreventDefault = false);

  /**
   * @brief Detach the current controls from the specified dom element.
   * @param ignored defines an ignored parameter kept for backward compatibility. If you want to
   * define the source input element, you can set engine.inputElement before calling
   * camera.attachControl
   */
  void detachControl(ICanvas* ignored = nullptr);

  /**
   * @brief Update the current camera state depending on the inputs that have been used this frame.
   * This is a dynamically created lambda to avoid the performance penalty of looping for inputs in
   * the render loop.
   */
  void checkInputs();

  /**
   * @brief Gets the class name of the current input.
   * @returns the class name
   */
  std::string getClassName() const;

  /**
   * @brief Get the friendly name associated with the input class.
   * @returns the input friendly name
   */
  std::string getSimpleName() const;

protected:
  /**
   * @brief Returns the camera.
   */
  virtual Camera* get_camera() const = 0;

public:
  /**
   * How fast is the camera moves in relation to X axis mouseWheel events.
   * Use negative value to reverse direction.
   */
  float wheelPrecisionX;

  /**
   * How fast is the camera moves in relation to Y axis mouseWheel events.
   * Use negative value to reverse direction.
   */
  float wheelPrecisionY;

  /**
   * How fast is the camera moves in relation to Z axis mouseWheel events.
   * Use negative value to reverse direction.
   */
  float wheelPrecisionZ;

  /**
   * Observable for when a mouse wheel move event occurs.
   */
  Observable<MouseWheelChangeEvent> onChangedObservable;

protected:
  /**
   * Incremental value of multiple mouse wheel movements of the X axis.
   * Should be zero-ed when read.
   */
  float _wheelDeltaX;

  /**
   * Incremental value of multiple mouse wheel movements of the Y axis.
   * Should be zero-ed when read.
   */
  float _wheelDeltaY;

  /**
   * Incremental value of multiple mouse wheel movements of the Z axis.
   * Should be zero-ed when read.
   */
  float _wheelDeltaZ;

private:
  std::function<void(PointerInfo* pointer, EventState& es)> _wheel;
  Observer<PointerInfo>::Ptr _observer;
  MouseWheelChangeEvent _tmpMouseWheelChangeEvent;

  /**
   * Firefox uses a different scheme to report scroll distances to other
   * browsers. Rather than use complicated methods to calculate the exact
   * multiple we need to apply, let's just cheat and use a constant.
   * https://developer.mozilla.org/en-US/docs/Web/API/WheelEvent/deltaMode
   * https://stackoverflow.com/questions/20110224/what-is-the-height-of-a-line-in-a-wheel-event-deltamode-dom-delta-line
   */
  const float _ffMultiplier;

  /**
   * Different event attributes for wheel data fall into a few set ranges.
   * Some relevant but dated date here:
   * https://stackoverflow.com/questions/5527601/normalizing-mousewheel-speed-across-browsers
   */
  const float _normalize;

}; // end of class BaseCameraMouseWheelInput

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_INPUTS_BASE_CAMERA_MOUSE_WHEEL_INPUT_H
