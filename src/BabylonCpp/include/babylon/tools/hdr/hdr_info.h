#ifndef BABYLON_TOOLS_HDR_HDR_INFO_H
#define BABYLON_TOOLS_HDR_HDR_INFO_H

#include <babylon/babylon_global.h>

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

  /**
   * Whether or not the header was sucessfully parsed.
   */
  bool isValid = false;

  /**
   * Header parse error message.
   */
  string_t errorMessage;

}; // end of struct HDRInfo

} // end of namespace BABYLON

#endif // end of BABYLON_TOOLS_HDR_HDR_INFO_H
