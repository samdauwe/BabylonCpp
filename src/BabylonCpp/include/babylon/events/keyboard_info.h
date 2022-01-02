#ifndef BABYLON_EVENTS_KEYBOARD_INFO_H
#define BABYLON_EVENTS_KEYBOARD_INFO_H

#include <babylon/babylon_api.h>
#include <babylon/core/structs.h>

namespace BABYLON {

/**
 * @brief This class is used to store keyboard related info for the onKeyboardObservable event.
 */
class BABYLON_SHARED_EXPORT KeyboardInfo {

public:
  /**
   * @brief Instantiates a new keyboard info.
   * This class is used to store keyboard related info for the onKeyboardObservable event.
   * @param type Defines the type of event (KeyboardEventTypes)
   * @param event Defines the related dom event
   */
  KeyboardInfo(unsigned int type, const KeyboardEvent& event);
  ~KeyboardInfo(); // = default

public:
  /**
   * Defines the type of event (KeyboardEventTypes)
   */
  unsigned int type;

  /**
   * Defines the related dom event
   */
  KeyboardEvent event;

}; // end of class KeyboardInfo

} // end of namespace BABYLON

#endif // end of BABYLON_EVENTS_KEYBOARD_INFO_H
