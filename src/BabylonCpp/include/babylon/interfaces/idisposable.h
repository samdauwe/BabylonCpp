#ifndef BABYLON_INTERFACES_IDISPOSABLE_H
#define BABYLON_INTERFACES_IDISPOSABLE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief IDisposable interface.
 */
struct BABYLON_SHARED_EXPORT IDisposable {

  /**
   * @brief Dispose the particle system and frees its associated resources.
   */
  virtual void dispose(bool doNotRecurse = false) = 0;

}; // end of struct IDisposable

} // end of namespace BABYLON

#endif // end of BABYLON_INTERFACES_IDISPOSABLE_H
