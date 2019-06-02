#ifndef BABYLON_MISC_HIGH_DYNAMIC_RANGE_HDR_INFO_H
#define BABYLON_MISC_HIGH_DYNAMIC_RANGE_HDR_INFO_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

/**
 * @brief Header information of HDR texture files.
 */
struct BABYLON_SHARED_EXPORT HDRInfo {

  /**
   * The height of the texture in pixels.
   */
  size_t height;

  /**
   * The width of the texture in pixels.
   */
  size_t width;

  /**
   * The index of the beginning of the data in the binary file.
   */
  size_t dataPosition;

}; // end of struct HDRInfo

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_HIGH_DYNAMIC_RANGE_HDR_INFO_H
