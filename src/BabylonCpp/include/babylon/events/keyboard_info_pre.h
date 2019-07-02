#ifndef BABYLON_EVENTS_KEYBOARD_INFO_PRE_H
#define BABYLON_EVENTS_KEYBOARD_INFO_PRE_H

#include <babylon/babylon_api.h>
#include <babylon/events/keyboard_info.h>

namespace BABYLON {

/**
 * @brief This class is used to store keyboard related info for the
 * onPreKeyboardObservable event.
 * Set the skipOnKeyboardObservable property to true if you want the engine to
 * stop any process after this event is triggered, even not calling
 * onKeyboardObservable
 */
class BABYLON_SHARED_EXPORT KeyboardInfoPre : public KeyboardInfo {

public:
  /**
   * @brief Instantiates a new keyboard pre info.
   * This class is used to store keyboard related info for the
   * onPreKeyboardObservable event.
   * @param type Defines the type of event (KeyboardEventTypes)
   * @param event Defines the related dom event
   */
  KeyboardInfoPre(unsigned int type, const KeyboardEvent& event);
  ~KeyboardInfoPre();

public:
  /**
   * Defines whether the engine should skip the next onKeyboardObservable
   * associated to this pre.
   */
  bool skipOnPointerObservable;

}; // end of class KeyboardInfoPre

} // end of namespace BABYLON

#endif // end of BABYLON_EVENTS_KEYBOARD_INFO_PRE_H
