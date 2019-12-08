#ifndef BABYLON_INPUTS_INPUT_MANAGER_H
#define BABYLON_INPUTS_INPUT_MANAGER_H

#include <functional>

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/core/structs.h>
#include <babylon/events/pointer_info.h>
#include <babylon/events/pointer_info_pre.h>
#include <babylon/inputs/click_info.h>
#include <babylon/misc/observable.h>
#include <babylon/misc/observer.h>

namespace BABYLON {

class AbstractActionManager;
class Engine;
class Scene;
using AbstractActionManagerPtr = std::shared_ptr<AbstractActionManager>;

/**
 * @brief Class used to manage all inputs for the scene.
 */
class BABYLON_SHARED_EXPORT InputManager {

public:
  /**
   * The distance in pixel that you have to move to prevent some events.
   */
  static unsigned int DragMovementThreshold;
  /**
   * Time in milliseconds to wait to raise long press events if button is still
   * pressed.
   */
  static milliseconds_t LongPressDelay;
  /**
   * Time in milliseconds with two consecutive clicks will be considered as a
   * double click.
   */
  static milliseconds_t DoubleClickDelay;
  /**
   * If you need to check double click without raising a single click at first
   * click, enable this flag.
   */
  static bool ExclusiveDoubleClickMode;

public:
  /**
   * @brief Creates a new InputManager.
   * @param scene defines the hosting scene
   */
  InputManager(Scene* scene);
  ~InputManager(); // = default

  /** Pointers handling **/

  /**
   * @brief Use this method to simulate a pointer move on a mesh.
   * The pickResult parameter can be obtained from a scene.pick or
   * scene.pickWithRay
   * @param pickResult pickingInfo of the object wished to simulate pointer
   * event on
   * @param pointerEventInit pointer event state to be used when simulating the
   * pointer event (eg. pointer id for multitouch)
   */
  void simulatePointerMove(std::optional<PickingInfo>& pickResult);

  /**
   * @brief Use this method to simulate a pointer down on a mesh.
   * The pickResult parameter can be obtained from a scene.pick or
   * scene.pickWithRay
   * @param pickResult pickingInfo of the object wished to simulate pointer
   * event on
   * @param pointerEventInit pointer event state to be used when simulating the
   * pointer event (eg. pointer id for multitouch)
   */
  void simulatePointerDown(std::optional<PickingInfo>& pickResult);

  /**
   * @brief Hidden
   */
  [[nodiscard]] bool _isPointerSwiping() const;

  /**
   * @brief Use this method to simulate a pointer up on a mesh.
   * The pickResult parameter can be obtained from a scene.pick or
   * scene.pickWithRay
   * @param pickResult pickingInfo of the object wished to simulate pointer
   * event on
   * @param pointerEventInit pointer event state to be used when simulating the
   * pointer event (eg. pointer id for multitouch)
   * @param doubleTap indicates that the pointer up event should be considered
   * as part of a double click (false by default)
   */
  void simulatePointerUp(std::optional<PickingInfo>& pickResult, bool doubleTap = false);

  /**
   * @brief Gets a boolean indicating if the current pointer event is captured
   * (meaning that the scene has already handled the pointer down).
   * @param pointerId defines the pointer id to use in a multi-touch scenario (0
   * by default)
   * @returns true if the pointer was captured
   */
  bool isPointerCaptured(int pointerId = 0);

  /**
   * @brief Attach events to the canvas (To handle actionManagers triggers and
   * raise onPointerMove, onPointerDown and onPointerUp
   * @param attachUp defines if you want to attach events to pointerup
   * @param attachDown defines if you want to attach events to pointerdown
   * @param attachMove defines if you want to attach events to pointermove
   */
  void attachControl(bool attachUp = true, bool attachDown = true, bool attachMove = true);

  /**
   * @brief Detaches all event handlers.
   */
  void detachControl();

  /**
   * @brief Force the value of meshUnderPointer.
   * @param mesh defines the mesh to use
   */
  void setPointerOverMesh(const AbstractMeshPtr& mesh);

  /**
   * @brief Gets the mesh under the pointer.
   * @returns a Mesh or null if no mesh is under the pointer
   */
  AbstractMeshPtr& getPointerOverMesh();

protected:
  /**
   * @brief Gets the mesh that is currently under the pointer.
   */
  AbstractMeshPtr& get_meshUnderPointer();

  /**
   * @brief Gets the pointer coordinates in 2D without any translation (ie.
   * straight out of the pointer event).
   */
  Vector2& get_unTranslatedPointer();

  /**
   * @brief Gets or sets the current on-screen X position of the pointer.
   */
  [[nodiscard]] int get_pointerX() const;

  /**
   * @brief Sets the current on-screen X position of the pointer.
   */
  void set_pointerX(int value);

  /**
   * @brief Gets the current on-screen Y position of the pointer.
   */
  [[nodiscard]] int get_pointerY() const;

  /**
   * @brief Sets the current on-screen Y position of the pointer
   */
  void set_pointerY(int value);

private:
  void _updatePointerPosition(const PointerEvent& evt);
  void _processPointerMove(std::optional<PickingInfo>& pickResult, const PointerEvent& evt);
  // Pointers handling
  void _setRayOnPointerInfo(PointerInfo& pointerInfo);
  bool _checkPrePointerObservable(const std::optional<PickingInfo>& pickResult,
                                  const PointerEvent& evt, PointerEventTypes type);
  void _processPointerDown(std::optional<PickingInfo>& pickResult, const PointerEvent& evt);
  void _processPointerUp(std::optional<PickingInfo>& pickResult, const PointerEvent& evt,
                         const ClickInfo& clickInfo);

public:
  /**
   * Gets the mesh that is currently under the pointer
   */
  ReadOnlyProperty<InputManager, AbstractMeshPtr> meshUnderPointer;

  /**
   * Gets the pointer coordinates in 2D without any translation (ie. straight
   * out of the pointer event)
   */
  ReadOnlyProperty<InputManager, Vector2> unTranslatedPointer;

  /**
   * Gets or sets the current on-screen X position of the pointer
   */
  Property<InputManager, int> pointerX;

  /**
   * Gets or sets the current on-screen Y position of the pointer
   */
  Property<InputManager, int> pointerY;

private:
  // Pointers
  std::string _wheelEventName;
  std::function<void(PointerEvent&& evt)> _onPointerMove;
  std::function<void(PointerEvent&& evt)> _onPointerDown;
  std::function<void(PointerEvent&& evt)> _onPointerUp;

  std::function<void(
    Observable<PointerInfoPre>& obs1, Observable<PointerInfo>& obs2, const PointerEvent& evt,
    const std::function<void(const ClickInfo& clickInfo, std::optional<PickingInfo> pickResult)>&
      cb)>
    _initClickEvent;
  std::function<AbstractActionManagerPtr(AbstractActionManagerPtr act, const ClickInfo& clickInfo)>
    _initActionManager;
  std::function<void(MouseButtonType btn, ClickInfo& clickInfo,
                     const std::function<void(const ClickInfo& clickInfo,
                                              const std::optional<PickingInfo>& pickResult)>& cb)>
    _delayedSimpleClick;
  milliseconds_t _delayedSimpleClickTimeout;
  milliseconds_t _previousDelayedSimpleClickTimeout;
  bool _meshPickProceed;

  MouseButtonType _previousButtonPressed;
  std::optional<PickingInfo> _currentPickResult;
  std::optional<PickingInfo> _previousPickResult;
  size_t _totalPointersPressed = 0;
  bool _doubleClickOccured;

  AbstractMeshPtr _pointerOverMesh;

  AbstractMeshPtr _pickedDownMesh;
  AbstractMeshPtr _pickedUpMesh;

  int _pointerX;
  int _pointerY;
  int _unTranslatedPointerX;
  int _unTranslatedPointerY;
  Vector2 _startingPointerPosition;
  Vector2 _previousStartingPointerPosition;
  high_res_time_point_t _startingPointerTime;
  high_res_time_point_t _previousStartingPointerTime;
  std::unordered_map<int, bool> _pointerCaptures;

  // Keyboard
  std::function<void(KeyboardEvent&& evt)> _onKeyDown;
  std::function<void(KeyboardEvent&& evt)> _onKeyUp;
  Observer<Engine>::Ptr _onCanvasFocusObserver;
  Observer<Engine>::Ptr _onCanvasBlurObserver;

  Scene* _scene;
  Vector2 _unTranslatedPointer;

}; // end of class InputManager

} // end of namespace BABYLON

#endif // end of BABYLON_INPUTS_INPUT_MANAGER_H
