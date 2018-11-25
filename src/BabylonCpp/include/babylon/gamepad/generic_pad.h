#ifndef BABYLON_GAMEPAD_GENERIC_PAD_H
#define BABYLON_GAMEPAD_GENERIC_PAD_H

#include <babylon/babylon_api.h>
#include <babylon/gamepad/gamepad.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

/**
 * @brief Represents a generic gamepad.
 */
class BABYLON_SHARED_EXPORT GenericPad : public Gamepad {

public:
  /**
   * @brief Initializes the generic gamepad.
   * @param id The id of the generic gamepad
   * @param index The index of the generic gamepad
   * @param browserGamepad The browser gamepad
   */
  GenericPad(const std::string& id, int index,
             const std::shared_ptr<IBrowserGamepad>& browserGamepad);
  virtual ~GenericPad() override;

  /**
   * @brief Callback triggered when a button has been pressed.
   * @param callback Called when a button has been pressed
   */
  void
  onbuttondown(const std::function<void(unsigned int buttonPressed)>& callback);

  /**
   * @brief Callback triggered when a button has been released.
   * @param callback Called when a button has been released
   */
  void
  onbuttonup(const std::function<void(unsigned int buttonPressed)>& callback);

  /**
   * @brief Updates the generic gamepad.
   */
  void update() override;

  /**
   * @brief Disposes the generic gamepad.
   */
  void dispose() override;

public:
  /**
   * Observable triggered when a button has been pressed
   */
  Observable<unsigned int> onButtonDownObservable;

  /**
   * Observable triggered when a button has been released
   */
  Observable<unsigned int> onButtonUpObservable;

private:
  unsigned int _setButtonValue(unsigned int newValue, unsigned int currentValue,
                               unsigned int buttonIndex);

private:
  Uint32Array _buttons;
  std::function<void(unsigned int buttonPressed)> _onbuttondown;
  std::function<void(unsigned int buttonPressed)> _onbuttonup;

}; // end of class GenericPad

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_GENERIC_PAD_H
