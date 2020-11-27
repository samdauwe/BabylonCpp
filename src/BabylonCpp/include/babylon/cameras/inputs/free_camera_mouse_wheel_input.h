#ifndef BABYLON_CAMERAS_INPUTS_FREE_CAMERA_MOUSE_WHEEL_INPUT_H
#define BABYLON_CAMERAS_INPUTS_FREE_CAMERA_MOUSE_WHEEL_INPUT_H

#include <babylon/cameras/free_camera.h>
#include <babylon/cameras/inputs/base_camera_mouse_wheel_input.h>

namespace BABYLON {

enum class _CameraProperty {
  MoveRelative,
  RotateRelative,
  MoveScene
}; // end of enum class _CameraProperty

/**
 * @brief Manage the mouse wheel inputs to control a free camera.
 * @see https://doc.babylonjs.com/how_to/customizing_camera_inputs
 */
class BABYLON_SHARED_EXPORT FreeCameraMouseWheelInput : public BaseCameraMouseWheelInput,
                                                        public ICameraInput<FreeCamera> {

public:
  /**
   * @brief Listen to mouse wheel events to control the camera.
   * @see https://doc.babylonjs.com/how_to/customizing_camera_inputs
   */
  FreeCameraMouseWheelInput();
  ~FreeCameraMouseWheelInput() override; // = default

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

  /**
   * @brief Attach the mouse control to the HTML DOM element.
   * @param noPreventDefault Defines whether events caught by the controls should call
   * preventdefault().
   */
  void attachControl(bool noPreventDefault = false) override;

  /**
   * @brief Detach the current controls from the specified dom element.
   * @param ignored defines an ignored parameter kept for backward compatibility. If you want to
   * define the source input element, you can set engine.inputElement before calling
   * camera.attachControl
   */
  void detachControl(ICanvas* ignored = nullptr) override;

  /**
   * @brief Called for each rendered frame.
   */
  void checkInputs() override;

protected:
  /**
   * @brief Returns the camera.
   */
  Camera* get_camera() const override;

  /**
   * @brief Set which movement axis (relative to camera's orientation) the mouse wheel's X axis
   * controls.
   * @param axis The axis to be moved. Set null to clear.
   */
  void set_wheelXMoveRelative(const std::optional<Coordinate>& axis);

  /**
   * @brief Get the configured movement axis (relative to camera's orientation) the mouse wheel's X
   * axis controls.
   * @returns The configured axis or null if none.
   */
  std::optional<Coordinate>& get_wheelXMoveRelative();

  /**
   * @brief Set which movement axis (relative to camera's orientation) the mouse wheel's Y axis
   * controls.
   * @param axis The axis to be moved. Set null to clear.
   */
  void set_wheelYMoveRelative(const std::optional<Coordinate>& axis);

  /**
   * @brief Get the configured movement axis (relative to camera's orientation) the mouse wheel's Y
   * axis controls.
   * @returns The configured axis or null if none.
   */
  std::optional<Coordinate>& get_wheelYMoveRelative();

  /**
   * @brief Set which movement axis (relative to camera's orientation) the mouse wheel's Z axis
   * controls.
   * @param axis The axis to be moved. Set null to clear.
   */
  void set_wheelZMoveRelative(const std::optional<Coordinate>& axis);

  /**
   * @brief Get the configured movement axis (relative to camera's orientation) the mouse wheel's Z
   * axis controls.
   * @returns The configured axis or null if none.
   */
  std::optional<Coordinate>& get_wheelZMoveRelative();

  /**
   * @brief Set which rotation axis (relative to camera's orientation) the mouse wheel's X axis
   * controls.
   * @param axis The axis to be moved. Set null to clear.
   */
  void set_wheelXRotateRelative(const std::optional<Coordinate>& axis);

  /**
   * @brief Get the configured rotation axis (relative to camera's orientation) the mouse wheel's X
   * axis controls.
   * @returns The configured axis or null if none.
   */
  std::optional<Coordinate>& get_wheelXRotateRelative();

  /**
   * @brief Set which rotation axis (relative to camera's orientation) the mouse wheel's Y axis
   * controls.
   * @param axis The axis to be moved. Set null to clear.
   */
  void set_wheelYRotateRelative(const std::optional<Coordinate>& axis);

  /**
   * @brief Get the configured rotation axis (relative to camera's orientation) the mouse wheel's Y
   * axis controls.
   * @returns The configured axis or null if none.
   */
  std::optional<Coordinate>& get_wheelYRotateRelative();

  /**
   * @brief Set which rotation axis (relative to camera's orientation) the mouse wheel's Z axis
   * controls.
   * @param axis The axis to be moved. Set null to clear.
   */
  void set_wheelZRotateRelative(const std::optional<Coordinate>& axis);

  /**
   * @brief Get the configured rotation axis (relative to camera's orientation) the mouse wheel's Z
   * axis controls.
   * @returns The configured axis or null if none.
   */
  std::optional<Coordinate>& get_wheelZRotateRelative();

  /**
   * @brief Set which movement axis (relative to the scene) the mouse wheel's X axis controls.
   * @param axis The axis to be moved. Set null to clear.
   */
  void set_wheelXMoveScene(const std::optional<Coordinate>& axis);

  /**
   * @brief Get the configured movement axis (relative to the scene) the mouse wheel's X axis
   * controls.
   * @returns The configured axis or null if none.
   */
  std::optional<Coordinate>& get_wheelXMoveScene();

  /**
   * @brief Set which movement axis (relative to the scene) the mouse wheel's Y axis controls.
   * @param axis The axis to be moved. Set null to clear.
   */
  void set_wheelYMoveScene(const std::optional<Coordinate>& axis);

  /**
   * @brief Get the configured movement axis (relative to the scene) the mouse wheel's Y axis
   * controls.
   * @returns The configured axis or null if none.
   */
  std::optional<Coordinate>& get_wheelYMoveScene();

  /**
   * @brief Set which movement axis (relative to the scene) the mouse wheel's Z axis controls.
   * @param axis The axis to be moved. Set null to clear.
   */
  void set_wheelZMoveScene(const std::optional<Coordinate>& axis);

  /**
   * @brief Get the configured movement axis (relative to the scene) the mouse wheel's Z axis
   * controls.
   * @returns The configured axis or null if none.
   */
  std::optional<Coordinate>& get_wheelZMoveScene();

private:
  /**
   * @brief Update the camera according to any configured properties for the 3 mouse-wheel axis.
   */
  void _updateCamera();

public:
  /**
   * Gets or sets the movement axis (relative to camera's orientation) the mouse wheel's X axis
   * controls.
   */
  Property<FreeCameraMouseWheelInput, std::optional<Coordinate>> wheelXMoveRelative;

  /**
   * Gets or sets the movement axis (relative to camera's orientation) the mouse wheel's Y axis
   * controls.
   */
  Property<FreeCameraMouseWheelInput, std::optional<Coordinate>> wheelYMoveRelative;

  /**
   * Gets or sets the movement axis (relative to camera's orientation) the mouse wheel's Z axis
   * controls.
   */
  Property<FreeCameraMouseWheelInput, std::optional<Coordinate>> wheelZMoveRelative;

  /**
   * Gets or sets the rotation axis (relative to camera's orientation) the mouse wheel's X axis
   * controls.
   */
  Property<FreeCameraMouseWheelInput, std::optional<Coordinate>> wheelXRotateRelative;

  /**
   * Gets or sets the rotation axis (relative to camera's orientation) the mouse wheel's Y axis
   * controls.
   */
  Property<FreeCameraMouseWheelInput, std::optional<Coordinate>> wheelYRotateRelative;

  /**
   * Gets or sets the rotation axis (relative to camera's orientation) the mouse wheel's Z axis
   * controls.
   */
  Property<FreeCameraMouseWheelInput, std::optional<Coordinate>> wheelZRotateRelative;

  /**
   * Gets or sets the movement axis (relative to the scene) the mouse wheel's X axis controls.
   */
  Property<FreeCameraMouseWheelInput, std::optional<Coordinate>> wheelXMoveScene;

  /**
   * Gets or sets the movement axis (relative to the scene) the mouse wheel's Y axis controls.
   */
  Property<FreeCameraMouseWheelInput, std::optional<Coordinate>> wheelYMoveScene;

  /**
   * Gets or sets the movement axis (relative to the scene) the mouse wheel's Z axis controls.
   */
  Property<FreeCameraMouseWheelInput, std::optional<Coordinate>> wheelZMoveScene;

private:
  Vector3 _moveRelative;
  Vector3 _rotateRelative;
  Vector3 _moveScene;

  /**
   * These are set to the desired default behaviour.
   */
  std::optional<_CameraProperty> _wheelXAction;
  std::optional<Coordinate> _wheelXActionCoordinate;
  std::optional<_CameraProperty> _wheelYAction;
  std::optional<Coordinate> _wheelYActionCoordinate;
  std::optional<_CameraProperty> _wheelZAction;
  std::optional<Coordinate> _wheelZActionCoordinate;
  std::optional<Coordinate> _nullCoordinate;

}; // end of class BaseCameraMouseWheelInput

} // end of namespace BABYLON

#endif // end of BABYLON_CAMERAS_INPUTS_BASE_CAMERA_MOUSE_WHEEL_INPUT_H
