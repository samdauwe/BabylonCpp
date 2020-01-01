#ifndef BABYLON_EVENTS_POINTER_EVENT_TYPES_H
#define BABYLON_EVENTS_POINTER_EVENT_TYPES_H

namespace BABYLON {

/**
 * @brief Gather the list of pointer event types as constants.
 */
enum class PointerEventTypes {
  /**
   * The pointerdown event is fired when a pointer becomes active. For mouse, it is fired when the
   * device transitions from no buttons depressed to at least one button depressed. For touch, it is
   * fired when physical contact is made with the digitizer. For pen, it is fired when the stylus
   * makes physical contact with the digitizer.
   */
  POINTERDOWN = 0x01,
  /**
   * The pointerup event is fired when a pointer is no longer active.
   */
  POINTERUP = 0x02,
  /**
   * The pointermove event is fired when a pointer changes coordinates.
   */
  POINTERMOVE = 0x04,
  /**
   * The pointerwheel event is fired when a mouse wheel has been rotated.
   */
  POINTERWHEEL = 0x08,
  /**
   * The pointerpick event is fired when a mesh or sprite has been picked by the pointer.
   */
  POINTERPICK = 0x10,
  /**
   * The pointertap event is fired when a the object has been touched and
   * released without drag.
   */
  POINTERTAP = 0x20,
  /**
   * The pointerdoubletap event is fired when a the object has been touched and released twice
   * without drag.
   */
  POINTERDOUBLETAP = 0x40,
}; // end of enum class PointerEventTypes

} // end of namespace BABYLON

#endif // end of BABYLON_EVENTS_POINTER_EVENT_TYPES_H
