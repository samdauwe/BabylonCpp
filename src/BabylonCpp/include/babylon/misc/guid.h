#ifndef BABYLON_MISC_GUID_H
#define BABYLON_MISC_GUID_H

#include <string>

#include <babylon/babylon_api.h>

namespace BABYLON {

/**
 * @brief Class used to manipulate GUIDs.
 */
struct BABYLON_SHARED_EXPORT GUID {

  /**
   * @brief Implementation from
   * http://stackoverflow.com/questions/105034/how-to-create-a-guid-uuid-in-javascript/2117523#answer-2117523
   * Be aware Math.random() could cause collisions, but:
   * "All but 6 of the 128 bits of the ID are randomly generated, which means
   * that for any two ids, there's a 1 in 2^^122 (or 5.3x10^^36) chance they'll
   * collide"
   * @returns a pseudo random id
   */
  static std::string RandomId();

}; // end of struct GUID

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_GUID_H
