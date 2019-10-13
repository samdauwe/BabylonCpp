#ifndef BABYLON_MISC_UNIQUE_ID_GENERATOR_H
#define BABYLON_MISC_UNIQUE_ID_GENERATOR_H

#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Helper class used to generate session unique ID.
 */
class BABYLON_SHARED_EXPORT UniqueIdGenerator {

private:
  // Statics
  static size_t _UniqueIdCounter;

public:
  /**
   * @brief Gets an unique (relatively to the current scene) Id.
   */
  static size_t UniqueId();

}; // end of class UniqueIdGenerator

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_UNIQUE_ID_GENERATOR_H
