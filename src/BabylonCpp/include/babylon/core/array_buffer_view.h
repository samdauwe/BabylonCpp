#ifndef BABYLON_CORE_ARRAY_BUFFER_VIEW_H
#define BABYLON_CORE_ARRAY_BUFFER_VIEW_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

/**
 * @brief ArrayBufferView is a helper type representing any of the following
 * TypedArray types:
 *  - Int8Array,
 *  - Uint8Array,
 *  - Int16Array,
 *  - Uint16Array,
 *  - Int32Array,
 *  - Uint32Array,
 *  - Float32Array,
 */
struct BABYLON_SHARED_EXPORT ArrayBufferView {

  ArrayBufferView();
  ArrayBufferView(const Int8Array& buffer);
  ArrayBufferView(const ArrayBuffer& arrayBuffer);
  ArrayBufferView(const Uint16Array& buffer);
  ArrayBufferView(const Uint32Array& buffer);
  ArrayBufferView(const Float32Array& buffer);
  ArrayBufferView(const ArrayBufferView& other);
  ArrayBufferView(ArrayBufferView&& other);
  ArrayBufferView& operator=(const ArrayBufferView& other);
  ArrayBufferView& operator=(ArrayBufferView&& other);
  ~ArrayBufferView(); // = default

  size_t byteLength() const;
  operator bool() const;

  size_t byteOffset;
  Int8Array int8Array;
  Uint8Array uint8Array;
  Int16Array int16Array;
  Uint16Array uint16Array;
  Int32Array int32Array;
  Uint32Array uint32Array;
  Float32Array float32Array;

}; // end of struct ArrayBufferView

} // end of namespace BABYLON

#endif // BABYLON_CORE_ARRAY_BUFFER_VIEW_H
