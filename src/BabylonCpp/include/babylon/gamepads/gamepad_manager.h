#ifndef BABYLON_GAMEPADS_GAMEPAD_MANAGER_H
#define BABYLON_GAMEPADS_GAMEPAD_MANAGER_H

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/gamepads/gamepad.h>
#include <babylon/misc/observable.h>

namespace BABYLON {

class Scene;

/**
 * @brief Manager for handling gamepads.
 */
class BABYLON_SHARED_EXPORT GamepadManager {

  friend class Scene;

public:
  /**
   * @brief Initializes the gamepad manager.
   * @param _scene BabylonJS scene
   */
  GamepadManager(Scene* scene);
  ~GamepadManager();

  /**
   * @brief The gamepads in the game pad manager.
   */
  std::vector<std::shared_ptr<Gamepad>>& gamepads();

  /**
   * @brief Get the gamepad controllers based on type.
   * @param type The type of gamepad controller
   * @returns Nullable gamepad
   */
  Gamepad* getGamepadByType(unsigned int type = Gamepad::XBOX);

  /**
   * @brief Disposes the gamepad manager.
   */
  void dispose();

  /**
   * @brief Hidden.
   */
  void _checkGamepadsStatus();

private:
  Gamepad* _addNewGamepad(Gamepad* gamepad);
  void _startMonitoringGamepads();
  void _stopMonitoringGamepads();
  void _updateGamepadObjects();

public:
  /** Hidden */
  bool _isMonitoring;

  /**
   * observable to be triggered when the gamepad controller has been connected
   */
  Observable<Gamepad> onGamepadConnectedObservable;

  /**
   * observable to be triggered when the gamepad controller has been
   * disconnected
   */
  Observable<Gamepad> onGamepadDisconnectedObservable;

private:
  Scene* _scene;

  std::vector<std::shared_ptr<Gamepad>> _babylonGamepads;
  bool _oneGamepadConnected;

  bool _gamepadEventSupported;

  std::function<void(Event&& evt)> _onGamepadConnectedEvent;
  std::function<void(Event&& evt)> _onGamepadDisconnectedEvent;

}; // end of class GamepadManager

} // end of namespace BABYLON

#endif // end of BABYLON_GAMEPADS_GAMEPAD_MANAGER_H
