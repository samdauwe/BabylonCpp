#ifndef BABYLON_TOOLS_EVENT_STATE_H
#define BABYLON_TOOLS_EVENT_STATE_H

#include <memory>

#include <babylon/babylon_api.h>
#include <babylon/core/any.h>

namespace BABYLON {

/**
 * @brief A class serves as a medium between the observable and its observers.
 */
class BABYLON_SHARED_EXPORT EventState {

public:
  using UPtr = std::unique_ptr<EventState>;

public:
  /**
   * @brief Create a new EventState.
   * @param mask defines the mask associated with this state
   * @param skipNextObservers defines a flag which will instruct the observable
   * to skip following observers when set to true
   * @param target defines the original target of the state
   * @param currentTarget defines the current target of the state
   */
  EventState(int mask, bool skipNextObservers = false, any* target = nullptr,
             any* currentTarget = nullptr);
  EventState(const EventState& other);            // Copy constructor
  EventState(EventState&& other);                 // Move constructor
  EventState& operator=(const EventState& other); // Copy assignment operator
  EventState& operator=(EventState&& other);      // Move assignment operator
  ~EventState();                                  // Destructor

  /**
   * @brief Initialize the current event state.
   * @param mask defines the mask associated with this state
   * @param skipNextObservers defines a flag which will instruct the observable
   * to skip following observers when set to true
   * @param target defines the original target of the state
   * @param currentTarget defines the current target of the state
   * @returns the current event state
   */
  EventState& initalize(int mask, bool skipNextObservers = false,
                        any* target = nullptr, any* currentTarget = nullptr);

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

  /**
   * The object that originally notified the event
   */
  any* target;

  /**
   * The current object in the bubbling phase
   */
  any* currentTarget;

  /**
   * This will be populated with the return value of the last function that was
   * executed. If it is the first function in the callback chain it will be the
   * event data.
   */
  any lastReturnValue;

}; // end of class Tools

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_EVENT_STATE_H
