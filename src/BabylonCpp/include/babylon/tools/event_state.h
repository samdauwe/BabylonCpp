#ifndef BABYLON_TOOLS_EVENT_STATE_H
#define BABYLON_TOOLS_EVENT_STATE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief A class serves as a medium between the observable and its observers.
 */
class BABYLON_SHARED_EXPORT EventState {

public:
  using UPtr = std::unique_ptr<EventState>;

public:
  /**
   * If the callback of a given Observer set skipNextObservers to true the
   * following observers will be ignored
   */
  EventState(int mask, bool skipNextObservers = false);
  EventState(const EventState& other);            // Copy constructor
  EventState(EventState&& other);                 // Move constructor
  EventState& operator=(const EventState& other); // Copy assignment operator
  EventState& operator=(EventState&& other);      // Move assignment operator
  ~EventState();                                  // Destructor

  EventState& initalize(int mask, bool skipNextObservers = false);

public:
  /**
   * An Observer can set this property to true to prevent subsequent observers
   * of being notified
   */
  bool skipNextObservers;

  /**
   * Get the mask value that were used to trigger the event corresponding to
   * this EventState object
   */
  int mask;

}; // end of class Tools

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_EVENT_STATE_H
