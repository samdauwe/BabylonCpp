#ifndef BABYLON_MESH_BUFFER_POINTER_H
#define BABYLON_MESH_BUFFER_POINTER_H

#include <babylon/babylon_global.h>

namespace BABYLON {

/**
* @brief
*/
struct BABYLON_SHARED_EXPORT BufferPointer {
  BufferPointer();
  BufferPointer(unsigned int indx, int size, unsigned int type, bool normalized,
                int stride, int offset, GL::IGLBuffer* buffer);
  ~BufferPointer();

  unsigned int indx;
  int size;
  unsigned int type;
  bool normalized;
  int stride;
  int offset;
  GL::IGLBuffer* buffer;
}; // end of struct BufferPointer

} // end of namespace BABYLON

#endif // end of BABYLON_MESH_BUFFER_POINTER_H
