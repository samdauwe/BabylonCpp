#ifndef BABYLON_MESH_DATA_BUFFER_H
#define BABYLON_MESH_DATA_BUFFER_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

/**
 * @brief Class used to store gfx data (like WebGLBuffer).
 */
struct BABYLON_SHARED_EXPORT DataBuffer {
  /**
   * Gets or sets the number of objects referencing this buffer
   */
  size_t references = 0;
  /** Gets or sets the size of the underlying buffer */
  size_t capacity = 0;
  /**
   * Gets or sets a boolean indicating if the buffer contains 32bits indices
   */
  bool is32Bits = false;

}; // end of struct DataBuffer

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_DATA_BUFFER_H
