#ifndef BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LOCK_OBJECT_H
#define BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LOCK_OBJECT_H

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Class used to provide lock mechanism.
 */
struct BABYLON_SHARED_EXPORT LockObject {

  /**
   * Gets or set if the lock is engaged
   */
  bool lock = false;

}; // end of struct LockObject

} // end of namespace BABYLON

#endif // end of BABYLON_INSPECTOR_COMPONENTS_ACTION_TABS_TABS_PROPERTY_GRIDS_LOCK_OBJECT_H
