#ifndef BABYLON_EVENTS_CLIPBOARD_INFO_H
#define BABYLON_EVENTS_CLIPBOARD_INFO_H

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>
#include <babylon/events/clipboard_event_types.h>

namespace BABYLON {

/**
 * @brief This class is used to store clipboard related info for the
 * onClipboardObservable event.
 */
class BABYLON_SHARED_EXPORT ClipboardInfo {

public:
  /**
   * @brief Creates an instance of ClipboardInfo.
   * @param type Defines the type of event (BABYLON.ClipboardEventTypes)
   * @param event Defines the related dom event
   */
  ClipboardInfo(unsigned int type, const ClipboardEvent& event);
  ~ClipboardInfo(); // = default

  /**
   * @brief Get the clipboard event's type from the keycode.
   * @param keyCode Defines the keyCode for the current keyboard event.
   * @return {number}
   */
  static int GetTypeFromCharacter(int keyCode);

public:
  /**
   * Defines the type of event (BABYLON.ClipboardEventTypes)
   */
  unsigned int type;

  /**
   * Defines the related dom event
   */
  ClipboardEvent event;

}; // end of class KeyboardInfo

} // end of namespace BABYLON

#endif // end of BABYLON_EVENTS_CLIPBOARD_INFO_H
