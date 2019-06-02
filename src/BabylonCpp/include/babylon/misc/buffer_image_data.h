#ifndef BABYLON_MISC_BUFFER_IMAGE_DATA_H
#define BABYLON_MISC_BUFFER_IMAGE_DATA_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

/**
 * @brief Defines One Image in the file. It requires only the position in the
 * file as well as the length.
 */
struct BABYLON_SHARED_EXPORT BufferImageData {
  /**
   * Length of the image data.
   */
  size_t length;
  /**
   * Position of the data from the null terminator delimiting the end of the
   * JSON.
   */
  size_t position;
}; // end of struct BufferImageData

} // end of namespace BABYLON

#endif // end of BABYLON_MISC_BUFFER_IMAGE_DATA_H
