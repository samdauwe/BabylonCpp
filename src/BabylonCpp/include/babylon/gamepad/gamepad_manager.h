#ifndef BABYLON_GAMEPAD_GAMEPAD_MANAGER_H
#define BABYLON_GAMEPAD_GAMEPAD_MANAGER_H

#include <babylon/babylon_global.h>
#include <babylon/core/structs.h>
#include <babylon/gamepad/gamepad.h>
#include <babylon/tools/observable.h>

namespace BABYLON {

class BABYLON_SHARED_EXPORT GamepadManager {

  friend class Scene;

public:
  GamepadManager(Scene* scene);
  ~GamepadManager();

  Gamepad* getGamepadByType(unsigned int type = Gamepad::XBOX);
  void dispose();

private:
  Gamepad* _addNewGamepad(Gamepad* gamepad);
  void _startMonitoringGamepads();
  void _stopMonitoringGamepads();
  void _checkGamepadsStatus();
  void _updateGamepadObjects();

public:
  Observable<Gamepad> onGamepadConnectedObservable;
  Observable<Gamepad> onGamepadDisconnectedObservable;

private:
  Scene* _scene;

  vector_t<shared_ptr_t<Gamepad>> _babylonGamepads;
  bool _oneGamepadConnected;

  bool _isMonitoring;
  bool _gamepadEventSupported;

  ::std::function<void(Event&& evt)> _onGamepadConnectedEvent;
  ::std::function<void(Event&& evt)> _onGamepadDisconnectedEvent;

}; // end of class GamepadManager

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPAD_GAMEPAD_MANAGER_H
