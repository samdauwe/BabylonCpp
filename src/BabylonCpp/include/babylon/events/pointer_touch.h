#ifndef BABYLON_EVENTS_POINTER_TOUCH_H
#define BABYLON_EVENTS_POINTER_TOUCH_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Data relating to a touch event on the screen.
 */
struct BABYLON_SHARED_EXPORT PointerTouch {
  /**
   * X coordinate of touch.
   */
  int x;
  /**
   * Y coordinate of touch.
   */
  int y;
  /**
   * Id of touch. Unique for each finger.
   */
  int pointerId;
  /**
   * Event type passed from DOM.
   */
  unsigned int type;
}; // end of struct PointerTouch

} // end of namespace BABYLON

#endif // end of BABYLON_EVENTS_POINTER_TOUCH_H
