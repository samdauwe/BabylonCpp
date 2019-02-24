#ifndef BABYLON_LOADING_GLTF_BINARY_READER_H
#define BABYLON_LOADING_GLTF_BINARY_READER_H

#include <babylon/babylon_api.h>
#include <babylon/babylon_common.h>
#include <babylon/core/data_view.h>

namespace BABYLON {
namespace GLTF2 {

class BABYLON_SHARED_EXPORT BinaryReader {

public:
  BinaryReader(const ArrayBuffer& arrayBuffer);
  ~BinaryReader();

  size_t getPosition() const;
  size_t getLength() const;
  uint32_t readUint32();
  Uint8Array readUint8Array(size_t length);
  void skipBytes(size_t length);

private:
  ArrayBuffer _arrayBuffer;
  DataView _dataView;
  size_t _byteOffset;

}; // end of class BinaryReader

} // end of namespace GLTF2
} // end of namespace BABYLON

#endif // end of BABYLON_LOADING_GLTF_BINARY_READER_H
