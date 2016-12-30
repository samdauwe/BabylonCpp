#ifndef BABYLON_INTERFACES_IDISPOSABLE_H
#define BABYLON_INTERFACES_IDISPOSABLE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief
 */
class BABYLON_SHARED_EXPORT IDisposable {

public:
  virtual void dispose(bool doNotRecurse = false) = 0;

}; // end of class IDisposable

} // end of namespace BABYLON

#endif // end of BABYLON_INTERFACES_IDISPOSABLE_H
