#ifndef BABYLON_MESHES_DATA_BUFFER_H
#define BABYLON_MESHES_DATA_BUFFER_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

/**
 * @brief Class used to store gfx data (like WebGLBuffer).
 */
template <class T>
class BABYLON_SHARED_EXPORT DataBuffer {

public:
  DataBuffer() : uniqueId{DataBuffer::_Counter++}
  {
  }
  virtual ~DataBuffer() = default;

  /**
   * Gets or sets the number of objects referencing this buffer
   */
  size_t references = 0;

  /**
   * Gets or sets the size of the underlying buffer
   */
  size_t capacity = 0;

  /**
   * Gets or sets a boolean indicating if the buffer contains 32bits indices
   */
  bool is32Bits = false;

  /**
   * @brief Gets the underlying buffer
   */
  virtual T underlyingResource()
  {
    return nullptr;
  }

  /**
   * @brief Gets the unique id of this buffer
   */
  const size_t uniqueId;

private:
  static inline size_t _Counter = 0;

}; // end of class DataBuffer

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_DATA_BUFFER_H
