#ifndef BABYLON_CAMERAS_INPUTS_FOLLOW_CAMERA_KEYBOARD_MOVE_INPUT_H
#define BABYLON_CAMERAS_INPUTS_FOLLOW_CAMERA_KEYBOARD_MOVE_INPUT_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/cameras/follow_camera.h>
#include <babylon/cameras/icamera_input.h>
#include <babylon/events/keyboard_info.h>

namespace BABYLON {

/**
 * @brief Manage the keyboard inputs to control the movement of a follow camera.
 * @see http://doc.babylonjs.com/how_to/customizing_camera_inputs
 */
class BABYLON_SHARED_EXPORT FollowCameraKeyboardMoveInput : public ICameraInput<FollowCamera> {

public:
  /**
   * @brief Instantiate the input.
   */
  FollowCameraKeyboardMoveInput();
  ~FollowCameraKeyboardMoveInput() override; // = default

  /**
   * @brief Attach the input controls to a specific dom element to get the input
   * from.
   * @param element Defines the element the controls should be listened from
   * @param noPreventDefault Defines whether event caught by the controls should
   * call preventdefault()
   * (https://developer.mozilla.org/en-US/docs/Web/API/Event/preventDefault)
   */
  void attachControl(ICanvas* canvas, bool noPreventDefault = false) override;

  /**
   * @brief Detach the current controls from the specified dom element.
   * @param element Defines the element to stop listening the inputs from
   */
  void detachControl(ICanvas* canvas) override;

  /**
   * @brief Update the current camera state depending on the inputs that have
   * been used this frame. This is a dynamically created lambda to avoid the
   * performance penalty of looping for inputs in the render loop.
   */
  void checkInputs() override;

  /**
   * @brief Gets the class name of the current input.
   * @returns the class name
   */
  std::string getClassName() const override;

  /**
   * @brief Get the friendly name associated with the input class.
   * @returns the input friendly name
   */
  std::string getSimpleName() const override;

private:
  /**
   * @brief Check if the pressed modifier keys (Alt/Ctrl/Shift) match those
   * configured to allow modification of the heightOffset value.
   */
  bool _modifierHeightOffset() const;

  /**
   * @brief Check if the pressed modifier keys (Alt/Ctrl/Shift) match those
   * configured to allow modification of the rotationOffset value.
   */
  bool _modifierRotationOffset() const;

  /**
   * @brief Check if the pressed modifier keys (Alt/Ctrl/Shift) match those
   * configured to allow modification of the radius value.
   */
  bool _modifierRadius() const;

public:
  /**
   * Defines the list of key codes associated with the up action (increase
   * heightOffset)
   */
  Int32Array keysHeightOffsetIncr;

  /**
   * Defines the list of key codes associated with the down action (decrease
   * heightOffset)
   */
  Int32Array keysHeightOffsetDecr;

  /**
   * Defines whether the Alt modifier key is required to move up/down (alter
   * heightOffset)
   */
  bool keysHeightOffsetModifierAlt;

  /**
   * Defines whether the Ctrl modifier key is required to move up/down (alter
   * heightOffset)
   */
  bool keysHeightOffsetModifierCtrl;

  /**
   * Defines whether the Shift modifier key is required to move up/down (alter
   * heightOffset)
   */
  bool keysHeightOffsetModifierShift;

  /**
   * Defines the list of key codes associated with the left action (increase
   * rotationOffset)
   */
  Int32Array keysRotationOffsetIncr;

  /**
   * Defines the list of key codes associated with the right action (decrease
   * rotationOffset)
   */
  Int32Array keysRotationOffsetDecr;

  /**
   * Defines whether the Alt modifier key is required to move left/right (alter
   * rotationOffset)
   */
  bool keysRotationOffsetModifierAlt;

  /**
   * Defines whether the Ctrl modifier key is required to move left/right (alter
   * rotationOffset)
   */
  bool keysRotationOffsetModifierCtrl;

  /**
   * Defines whether the Shift modifier key is required to move left/right
   * (alter rotationOffset)
   */
  bool keysRotationOffsetModifierShift;

  /**
   * Defines the list of key codes associated with the zoom-in action (decrease
   * radius)
   */
  Int32Array keysRadiusIncr;

  /**
   * Defines the list of key codes associated with the zoom-out action (increase
   * radius)
   */
  Int32Array keysRadiusDecr;

  /**
   * Defines whether the Alt modifier key is required to zoom in/out (alter
   * radius value)
   */
  bool keysRadiusModifierAlt;

  /**
   * Defines whether the Ctrl modifier key is required to zoom in/out (alter
   * radius value)
   */
  bool keysRadiusModifierCtrl;

  /**
   * Defines whether the Shift modifier key is required to zoom in/out (alter
   * radius value)
   */
  bool keysRadiusModifierShift;

  /**
   * Defines the rate of change of heightOffset.
   */
  float heightSensibility;

  /**
   * Defines the rate of change of rotationOffset.
   */
  float rotationSensibility;

  /**
   * Defines the rate of change of radius.
   */
  float radiusSensibility;

private:
  ICanvas* _canvas;
  bool _noPreventDefault;
  Int32Array _keys;
  bool _ctrlPressed;
  bool _altPressed;
  bool _shiftPressed;
  Observer<Engine>::Ptr _onCanvasBlurObserver;
  Observer<KeyboardInfo>::Ptr _onKeyboardObserver;
  Engine* _engine;
  Scene* _scene;

}; // end of class ArcRotateCameraKeyboardMoveInput

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_INPUTS_ARC_ROTATE_CAMERA_KEYBOARD_MOVE_INPUT_H
