#ifndef BABYLON_MESHES_BUFFER_POINTER_H
#define BABYLON_MESHES_BUFFER_POINTER_H

#include <memory>

#include <babylon/babylon_api.h>

namespace BABYLON {

class WebGLDataBuffer;
using WebGLDataBufferPtr = std::shared_ptr<WebGLDataBuffer>;

/**
 * @brief Keeps track of all the buffer info used in engine.
 */
struct BABYLON_SHARED_EXPORT BufferPointer {
  BufferPointer();
  BufferPointer(bool active, unsigned int indx, int size, unsigned int type, bool normalized,
                int stride, int offset, const WebGLDataBufferPtr& buffer);
  ~BufferPointer(); // = default

  bool active;
  unsigned int index;
  int size;
  unsigned int type;
  bool normalized;
  int stride;
  int offset;
  WebGLDataBufferPtr buffer;
}; // end of struct BufferPointer

} // end of namespace BABYLON

#endif // end of BABYLON_MESHES_BUFFER_POINTER_H
