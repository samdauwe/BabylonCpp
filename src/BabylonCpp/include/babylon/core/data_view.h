#ifndef BABYLON_CORE_DATA_VIEW_H
#define BABYLON_CORE_DATA_VIEW_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>

namespace BABYLON {

/**
 * @brief The DataView view provides a low-level interface for reading and
 * writing multiple number types in an ArrayBuffer irrespective of the
 * platform's endianness.
 */
class BABYLON_SHARED_EXPORT DataView {

public:
  DataView();

  /**
   * @brief Constructor
   * @param buffer An existing ArrayBuffer to use as the storage for the new
   * DataView object.
   */
  DataView(const ArrayBuffer& buffer);

  /**
   * @brief Constructor
   * @param buffer An existing ArrayBuffer to use as the storage for the new
   * DataView object.
   * @param byteOffset The offset, in bytes, to the first byte in the specified
   * buffer for the new view to reference. If not specified, the view of the
   * buffer will start with the first byte.
   * @param byteLength The number of elements in the byte array. If unspecified,
   * length of the view will match the buffer's length.
   */
  DataView(const ArrayBuffer& buffer, size_t byteOffset, size_t byteLength);

  DataView(const DataView& other);            // Copy constructor
  DataView(DataView&& other);                 // Move constructor
  DataView& operator=(const DataView& other); // Copy assignment operator
  DataView& operator=(DataView&& other);      // Move assignment operator
  ~DataView();                                // Destructor

  /**
   * @brief Gets a signed 8-bit integer (byte) at the specified byte offset from
   * the start of the DataView.
   * @param byteOffset The offset, in byte, from the start of the view where to
   * read the data.
   * @param littleEndian
   * @return A signed 8-bit integer number.
   */
  int8_t getInt8(size_t byteOffset) const;

  /**
   * @brief Gets an unsigned 8-bit integer (unsigned byte) at the specified byte
   * offset from the start of the DataView.
   * @param byteOffset The offset, in byte, from the start of the view where to
   * read the data.
   * @param littleEndian
   * @return An unsigned 8-bit integer number.
   */
  uint8_t getUint8(size_t byteOffset) const;

  /**
   * @brief Gets a signed 16-bit integer (short) at the specified byte offset
   * from the start of the DataView.
   * @param byteOffset The offset, in byte, from the start of the view where to
   * read the data.
   * @param littleEndian Indicates whether the 16-bit int is stored in little-
   * or big-endian format. If false or undefined, a big-endian value is read.
   * @return A signed 16-bit integer number.
   */
  int16_t getInt16(size_t byteOffset, bool littleEndian = true) const;

  /**
   * @brief Gets a signed 32-bit integer (signed short) at the specified byte
   * offset from the start of the DataView.
   * @param byteOffset The offset, in byte, from the start of the view where to
   * read the data.
   * @param littleEndian Indicates whether the 32-bit int is stored in little-
   * or big-endian format. If false or undefined, a big-endian value is read.
   * @return A signed 32-bit integer number.
   */
  int32_t getInt32(size_t byteOffset, bool littleEndian = true) const;

  /**
   * @brief Gets an unsigned 16-bit integer (unsigned short) at the specified
   * byte offset from the start of the DataView.
   * @param byteOffset The offset, in byte, from the start of the view where to
   * read the data.
   * @param littleEndian Indicates whether the 16-bit int is stored in little-
   * or big-endian format. If false or undefined, a big-endian value is read.
   * @return An unsigned 16-bit integer number.
   */
  uint16_t getUint16(size_t byteOffset, bool littleEndian = true) const;

  /**
   * @brief Gets an unsigned 32-bit integer (unsigned short) at the specified
   * byte offset from the start of the DataView.
   * @param byteOffset The offset, in byte, from the start of the view where to
   * read the data.
   * @param littleEndian Indicates whether the 16-bit int is stored in little-
   * or big-endian format. If false or undefined, a big-endian value is read.
   * @return An unsigned 32-bit integer number.
   */
  uint32_t getUint32(size_t byteOffset, bool littleEndian = true) const;

  /**
   * @brief Gets a signed 32-bit float (float) at the specified byte offset from
   * the start of the DataView.
   * @param byteOffset The offset, in byte, from the start of the view where to
   * read the data.
   * @param littleEndian Indicates whether the 32-bit float is stored in little-
   * or big-endian format. If false or undefined, a big-endian value is read.
   * @return A signed 32-bit float number.
   */
  float getFloat32(size_t byteOffset, bool littleEndian = true) const;

  /**
   * @brief Revert the endianness of a value.
   * Not as fast hardware based, but will probably never need to use
   * @param val defines the value to convert
   * @returns the new value
   */
  static int switchEndianness(int val);

private:
  /**
   * The ArrayBuffer referenced by this view
   */
  ArrayBuffer _buffer;

  /**
   * The length (in bytes) of this view from the start of its ArrayBuffer
   */
  size_t _byteLength;

  /**
   * The offset (in bytes) of this view from the start of its ArrayBuffer
   */
  size_t _byteOffset;

}; // end of class DataView

} // end of namespace BABYLON

#endif // end of BABYLON_CORE_DATA_VIEW_H
