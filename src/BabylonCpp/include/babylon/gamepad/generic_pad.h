#ifndef BABYLON_GAMEPAD_GENERIC_PAD_H
#define BABYLON_GAMEPAD_GENERIC_PAD_H

#include <babylon/babylon_api.h>
#include <babylon/gamepad/gamepad.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT GenericPad : public Gamepad {

public:
  GenericPad(const std::string& id, int index,
             const std::shared_ptr<IBrowserGamepad>& browserGamepad);
  virtual ~GenericPad() override;

  void setOnbuttondown(
    const std::function<void(unsigned int buttonPressed)>& callback);
  void
  onbuttonup(const std::function<void(unsigned int buttonPressed)>& callback);
  void update() override;
  void dispose() override;

private:
  unsigned int _setButtonValue(unsigned int newValue, unsigned int currentValue,
                               unsigned int buttonIndex);

private:
  Uint32Array _buttons;
  std::function<void(unsigned int buttonPressed)> _onbuttondown;
  std::function<void(unsigned int buttonPressed)> _onbuttonup;

  Observable<unsigned int> onButtonDownObservable;
  Observable<unsigned int> onButtonUpObservable;

}; // end of class GenericPad

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_GENERIC_PAD_H
