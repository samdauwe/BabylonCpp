#ifndef BABYLON_EVENTS_DEVICE_INPUT_EVENTS_H
#define BABYLON_EVENTS_DEVICE_INPUT_EVENTS_H

#include <functional>
#include <memory>
#include <optional>
#include <string>

#include <babylon/babylon_fwd.h>

namespace BABYLON {

class ICanvas;
FWD_STRUCT_SPTR(IKeyboardEvent)

/**
 * @brief Event Types
 */
enum class DeviceInputEventType {
  // Pointers
  /** PointerMove */
  PointerMove,
  /** PointerDown */
  PointerDown,
  /** PointerUp */
  PointerUp
}; // end of enum class DeviceInputEventType

/**
 * @brief Native friendly interface for Event Object.
 */
struct IEvent {
  // Properties
  /**
   * Current target for an event
   */
  ICanvas* currentTarget = nullptr;

  /**
   * Alias for target
   * @deprecated
   */
  ICanvas* srcElement = nullptr;

  /**
   * Type of event
   */
  std::string type;

  /**
   * Reference to object where object was dispatched
   */
  void* target = nullptr;

  // Methods
  /**
   * Tells user agent what to do when not explicitly handled
   */
  std::function<void()> preventDefault = nullptr;
}; // end of struct IEvent

/**
 * @brief Native friendly interface for UIEvent Object.
 */
struct IUIEvent : public IEvent {
  // Properties
  /**
   * Provides current click count
   */
  int detail;

  /**
   * Horizontal coordinate of event
   */
  int pageX;

  /**
   * Vertical coordinate of event
   */
  int pageY;
}; // end of struct IUIEvent

/**
 * @brief Native friendly interface for KeyboardEvent Object
 */
struct IKeyboardEvent : public IUIEvent {
  // Properties
  /**
   * Status of Alt key being pressed
   */
  bool altKey;

  /**
   * Unicode value of character pressed
   * @deprecated
   */
  std::optional<int> charCode = std::nullopt;

  /**
   * Code for key based on layout
   */
  std::string code;

  /**
   * Status of Ctrl key being pressed
   */
  bool ctrlKey;

  /**
   * String representation of key
   */
  std::string key;

  /**
   * ASCII value of key
   * @deprecated
   */
  int keyCode;

  /**
   * Status of Meta key (eg. Windows key) being pressed
   */
  int metaKey;

  /**
   * Status of Shift key being pressed
   */
  int shiftKey;
}; // end of struct IKeyboardEvent

/**
 * @brief Native friendly interface for MouseEvent Object
 */
struct IMouseEvent : public IUIEvent {
  // Properties
  /**
   * Status of Alt key being pressed
   */
  bool altKey;

  /**
   * Value of single mouse button pressed
   */
  int button;

  /**
   * Value of all mouse buttons pressed
   */
  int buttons;

  /**
   * Current X coordinate
   */
  int clientX;

  /**
   * Current Y coordinate
   */
  int clientY;

  /**
   * Status of Ctrl key being pressed
   */
  bool ctrlKey;

  /**
   * Status of Meta key (eg. Windows key) being pressed
   */
  bool metaKey;

  /**
   * Delta of movement on X axis
   */
  int movementX;

  /**
   * Delta of movement on Y axis
   */
  int movementY;

  /**
   * Delta of movement on X axis
   */
  std::optional<int> mozMovementX = std::nullopt;

  /**
   * Delta of movement on Y axis
   */
  std::optional<int> mozMovementY = std::nullopt;

  /**
   * Delta of movement on X axis
   */
  std::optional<int> msMovementX = std::nullopt;

  /**
   * Delta of movement on Y axis
   */
  std::optional<int> msMovementY = std::nullopt;

  /**
   * Current coordinate of X within container
   */
  int offsetX;

  /**
   * Current coordinate of Y within container
   */
  int offsetY;

  /**
   * Status of Shift key being pressed
   */
  bool shiftKey;

  /**
   * Delta of movement on X axis
   */
  std::optional<int> webkitMovementX = std::nullopt;

  /**
   * Delta of movement on Y axis
   */
  std::optional<int> webkitMovementY = std::nullopt;

  /**
   * Alias of clientX
   */
  int x;

  /**
   * Alias of clientY
   */
  int y;
}; // end of struct IMouseEvent

/**
 * @brief Native friendly interface for PointerEvent Object.
 */
struct IPointerEvent : public IMouseEvent {
  // Properties
  /**
   * Pointer Event ID
   */
  int pointerId;

  /**
   * Type of pointer
   */
  std::string pointerType;
}; // end of struct IPointerEvent

/**
 * Native friendly interface for WheelEvent Object
 */
struct IWheelEvent : public IMouseEvent {
  // Properties
  /**
   * Units for delta value
   */
  int deltaMode;

  /**
   * Horizontal scroll delta
   */
  int deltaX;

  /**
   * Vertical scroll delta
   */
  int deltaY;

  /**
   * Z-Axis scroll delta
   */
  int deltaZ;
}; // end of struct IWheelEvent

/**
 * @brief Constants used for Events.
 */
struct EventConstants {
  /**
   * Pixel delta for Wheel Events (Default)
   */
  static constexpr unsigned int DOM_DELTA_PIXEL = 0x00;

  /**
   * Line delta for Wheel Events
   */
  static constexpr unsigned int DOM_DELTA_LINE = 0x01;

  /**
   * Page delta for Wheel Events
   */
  static constexpr unsigned int DOM_DELTA_PAGE = 0x02;
}; // end of struct EventConstants

} // end of namespace BABYLON

#endif // end of BABYLON_EVENTS_CLIPBOARD_INFO_H
