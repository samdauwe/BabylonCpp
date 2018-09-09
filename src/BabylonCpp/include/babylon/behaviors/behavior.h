#ifndef BABYLON_BEHAVIORS_BEHAVIOR_H
#define BABYLON_BEHAVIORS_BEHAVIOR_H

#include <memory>
#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Interface used to define a behavior.
 */
template <class T>
struct BABYLON_SHARED_EXPORT Behavior {
  /**
   * gets or sets behavior's name
   */
  std::string name;

  /**
   * @brief Function called when the behavior needs to be initialized (after
   * attaching it to a target).
   */
  virtual void init() = 0;
  /**
   * @brief Called when the behavior is attached to a target.
   * @param target defines the target where the behavior is attached to
   */
  virtual void attach(const std::shared_ptr<T>& target) = 0;
  /**
   * @brief Called when the behavior is detached from its target.
   */
  virtual void detach() = 0;

}; // end of struct Behavior<T>

} // end of namespace BABYLON

#endif // end of BABYLON_BEHAVIORS_BEHAVIOR_H
