#ifndef BABYLON_ENGINE_BUFFER_POINTER_H
#define BABYLON_ENGINE_BUFFER_POINTER_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct BABYLON_SHARED_EXPORT BufferPointer {
  bool active;
  std::size_t index;
  std::size_t size;
  std::size_t type;
  bool normalized;
  std::size_t stride;
  std::size_t offset;
  GL::IGLBuffer* buffer;
}; // end of struct PointerInfo

} // end of namespace BABYLON

#endif // end of BABYLON_ENGINE_BUFFER_POINTER_H
