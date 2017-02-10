#ifndef BABYLON_CORE_ARRAY_BUFFER_VIEW_H
#define BABYLON_CORE_ARRAY_BUFFER_VIEW_H

#include <babylon/babylon_global.h>

namespace BABYLON {

struct ArrayBufferView {
  Int8Array int8Array;
  Uint8Array uint8Array;
  Int16Array int16Array;
  Uint16Array uint16Array;
  Float32Array float32Array;
}; // end of struct ArrayBufferView

} // end of namespace BABYLON

#endif // end of BABYLON_CORE_ARRAY_BUFFER_VIEW_H