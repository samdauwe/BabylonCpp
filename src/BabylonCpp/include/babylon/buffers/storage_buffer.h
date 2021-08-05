#ifndef BABYLON_BUFFERS_STORAGE_BUFFER_H
#define BABYLON_BUFFERS_STORAGE_BUFFER_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/buffers/data_buffer.h>
#include <babylon/engines/thin_engine.h>

namespace BABYLON {

/**
 * @brief This class is a small wrapper around a native buffer that can be read and/or written.
 */
template <class T>
class BABYLON_SHARED_EXPORT StorageBuffer {

private:
  ThinEngine* _engine;
  DataBuffer<T> _buffer;
  size_t _bufferSize;
  unsigned int _creationFlags;

public:
  /**
   * @brief Creates a new storage buffer instance.
   * @param engine The engine the buffer will be created inside
   * @param size The size of the buffer in bytes
   * @param creationFlags flags to use when creating the buffer (see
   * Constants.BUFFER_CREATIONFLAG_XXX). The BUFFER_CREATIONFLAG_STORAGE flag will be automatically
   * added.
   */
  StorageBuffer(ThinEngine* engine, size_t size,
                unsigned int creationFlags = Constants::BUFFER_CREATIONFLAG_READWRITE)
  {
    _engine = engine;
    // _engine._storageBuffers.push(this);
    _create(size, creationFlags);
  }
  ~StorageBuffer() = default;

  /**
   * @brief hidden
   */
  void _rebuild()
  {
    _create(_bufferSize, _creationFlags);
  }

  /**
   * @brief Gets underlying native buffer.
   * @returns underlying native buffer
   */
  DataBuffer<T>& getBuffer()
  {
    return _buffer;
  }

  /**
   * @brief Updates the storage buffer.
   * @param data the data used to update the storage buffer
   * @param byteOffset the byte offset of the data (optional)
   * @param byteLength the byte length of the data (optional)
   */
  void update(const ArrayBuffer& /*data*/, const std::optional<int>& /*byteOffset*/ = std::nullopt,
              const std::optional<size_t>& /*byteLength*/ = std::nullopt)
  {
    if (!_buffer) {
      return;
    }

    // _engine.updateStorageBuffer(_buffer, data, byteOffset, byteLength);
  }

  /**
   * @brief Reads data from the storage buffer.
   * @param offset The offset in the storage buffer to start reading from (default: 0)
   * @param size  The number of bytes to read from the storage buffer (default: capacity of the
   * buffer)
   * @param buffer The buffer to write the data we have read from the storage buffer to (optional)
   * @returns If not undefined, returns the (promise) buffer (as provided by the 4th parameter)
   * filled with the data, else it returns a (promise) Uint8Array with the data read from the
   * storage buffer
   */
  ArrayBufferView read(const std::optional<int>& /*offset*/             = std::nullopt,
                       const std::optional<size_t>& /*size*/            = std::nullopt,
                       const std::optional<ArrayBufferView>& /*buffer*/ = std::nullopt)
  {
    // return _engine.readFromStorageBuffer(_buffer, offset, size, buffer);
    return ArrayBufferView{};
  }

  /**
   * @brief Disposes the storage buffer.
   */
  void dispose()
  {
#if 0
    const storageBuffers = _engine._storageBuffers;
    let index            = storageBuffers.indexOf(this);

    if (index != -1) {
      storageBuffers[index] = storageBuffers[storageBuffers.length - 1];
      storageBuffers.pop();
    }

    _engine._releaseBuffer(_buffer);
    _buffer = null as any;
#endif
  }

private:
  void _create(size_t size, unsigned int creationFlags)
  {
    _bufferSize    = size;
    _creationFlags = creationFlags;
    // _buffer        = _engine.createStorageBuffer(size, creationFlags);
  }

}; // end of class StorageBuffer

} // end of namespace BABYLON

#endif // end of BABYLON_BUFFERS_STORAGE_BUFFER_H
