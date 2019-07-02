#ifndef BABYLON_EVENTS_CLIPBOARD_EVENT_TYPES_H
#define BABYLON_EVENTS_CLIPBOARD_EVENT_TYPES_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Gather the list of clipboard event types as constants.
 */
struct BABYLON_SHARED_EXPORT ClipboardEventTypes {
  /**
   * The clipboard event is fired when a copy command is active (pressed).
   */
  static constexpr unsigned int COPY = 0x01;
  /**
   *  The clipboard event is fired when a cut command is active (pressed).
   */
  static constexpr unsigned int CUT = 0x02;
  /**
   * The clipboard event is fired when a paste command is active (pressed).
   */
  static constexpr unsigned int PASTE = 0x03;
}; // end of struct ClipboardEventTypes

} // end of namespace BABYLON

#endif // end of BABYLON_EVENTS_CLIPBOARD_EVENT_TYPES_H
