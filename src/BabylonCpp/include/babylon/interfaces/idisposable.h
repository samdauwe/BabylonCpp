#ifndef BABYLON_INTERFACES_IDISPOSABLE_H
#define BABYLON_INTERFACES_IDISPOSABLE_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
 * @brief IDisposable interface.
 */
struct BABYLON_SHARED_EXPORT IDisposable {

  /**
   * @brief Releases resources associated with this node.
   * @param doNotRecurse Set to true to not recurse into each children (recurse
   * into each children by default)
   * @param disposeMaterialAndTextures Set to true to also dispose referenced
   * materials and textures (false by default)
   */
  virtual void dispose(bool doNotRecurse               = false,
                       bool disposeMaterialAndTextures = false)
    = 0;

}; // end of struct IDisposable

} // end of namespace BABYLON

#endif // end of BABYLON_INTERFACES_IDISPOSABLE_H
