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
class BABYLON_SHARED_EXPORT ArrayBufferView {

public:
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

  void clear();
  size_t byteLength() const;
  operator bool() const;

  Uint8Array& buffer();
  const Uint8Array& buffer() const;
  Uint8Array& uint8Array();
  const Uint8Array& uint8Array() const;
  Int8Array int8Array() const;
  Int16Array int16Array() const;
  Uint16Array uint16Array() const;
  Int32Array int32Array() const;
  Uint32Array uint32Array() const;
  Float32Array float32Array() const;

public:
  size_t byteOffset = 0;

private:
  Uint8Array _uint8Array;

}; // end of class ArrayBufferView

} // end of namespace BABYLON

#endif // BABYLON_CORE_ARRAY_BUFFER_VIEW_H
